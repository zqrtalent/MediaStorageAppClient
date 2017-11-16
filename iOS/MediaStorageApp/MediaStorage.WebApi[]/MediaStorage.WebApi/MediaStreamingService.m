//
//  MediaStreamingService.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/2/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "MediaStreamingService.h"
#include "Utility/GrowableMemory.h"

typedef NS_ENUM(int, ServiceMethodType)
{
    NoCall = 0,
    AuthenticateCall,
    GetLibraryInfoCall,
    GetImageResourceCall,
    AudioPacketsByTimeCall,
    AudioPacketsByOffsetCall
};

@interface MediaStreamingService()<NSURLSessionDelegate, NSURLSessionTaskDelegate, NSURLSessionDownloadDelegate>
{
    ServiceMethodType _serviceCall;
    id<MediaStreamingServiceDelegate> _delegate;
    std::unique_ptr<GrowableMemory> _memBuffer;
}

@property(nonatomic, strong) NSURLSession* urlSession;
@property(nonatomic, strong) NSString* host;
@property(nonatomic, strong) NSURLSessionDataTask* runningTask;

@end

@implementation MediaStreamingService

-(void)dealloc{
    //auto urlSes = self.urlSession;
    self.urlSession = nil;
    //[urlSes dealloc];
    
    self.host = nil;
    self.runningTask = nil;
    _memBuffer.reset();
}

-(BOOL)IsInProgress{
    return (_runningTask != nil && _runningTask.state == NSURLSessionTaskStateRunning);
}

-(void)ForceToStop{
    if([self IsInProgress])
        [self.urlSession invalidateAndCancel];
    
    self.urlSession = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:self delegateQueue:nil];
    self.runningTask = nil;
    _serviceCall = NoCall;
    _memBuffer.reset(new GrowableMemory(0, 1024, false));
    //_memBuffer = std::unique_ptr<GrowableMemory>(new GrowableMemory(0, 1024, false));
}

-(MediaStreamingService*)init:(id<MediaStreamingServiceDelegate>)delegate{
    //self.host = @"http://45.35.50.10:81";
    self.host = @"https://localhost:5001";
    _delegate = delegate;
    self.urlSession = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration] delegate:self delegateQueue:nil];
    _serviceCall = NoCall;
    _memBuffer = std::unique_ptr<GrowableMemory>(new GrowableMemory(0, 1024, false));
    return self;
}

-(void)Authenticate:(NSString*)userName Password:(NSString*)password{
    NSString* hash = @"temp"; // Calculate hsh of username and password
    NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/streaming/api/v1/auth/%@/%@/%@", _host, userName, password, hash]];
    self.runningTask = [self.urlSession dataTaskWithURL:url];
    _serviceCall = AuthenticateCall;
    [self.runningTask resume];
}

-(void)GetLibraryInfo:(NSString*)sessionKey{
    NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/streaming/api/v1/%@/library/info", _host, sessionKey]];
    self.runningTask = [self.urlSession dataTaskWithURL:url];
    _serviceCall = GetLibraryInfoCall;
    [self.runningTask resume];

}

-(void)GetImageResource:(NSString*)sessionKey ImageId:(NSString*)imageId SizeType:(NSString*)sizeType{
    NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/streaming/api/v1/%@/image/%@/%@",
                                       _host, sessionKey, imageId, sizeType]];
    self.runningTask = [self.urlSession dataTaskWithURL:url];
    _serviceCall = GetImageResourceCall;
    [_runningTask resume];
}

-(void)AudioPacketsByOffset:(NSString*)sessionKey SongId:(NSString*)songId byOffset:(int)offset Packets:(int)numPackets{
    NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/streaming/api/v1/%@/audiopackets/%@/offset/%d/%d", _host, sessionKey, songId, offset, numPackets]];
    //offset = 8200;
    self.runningTask = [self.urlSession dataTaskWithURL:url];
    _serviceCall = AudioPacketsByOffsetCall;
    [_runningTask resume];

}

-(void)AudioPacketsByTime:(NSString*)sessionKey SongId:(NSString*)songId byMilliSecond:(int)msec Packets:(int)numPackets{
    NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"%@/streaming/api/v1/%@/audiopackets/%@/time/%d/%d", _host, sessionKey, songId, msec, numPackets]];
    self.runningTask = [self.urlSession dataTaskWithURL:url];
    _serviceCall = AudioPacketsByTimeCall;
    [_runningTask resume];
}

-(void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data{
    //if(_delegate == nil) return;
    long long expectedContentLength = 0;
    NSString* mimeType = nil;
    auto resp = (NSHTTPURLResponse*)[dataTask response];
    if(resp != nil){
        if(resp.statusCode == 404){
//            dispatch_async(dispatch_get_main_queue(), ^(){
//                [_delegate OnAudioPacketsByOffsetResponse:(MediaPackets*)pObject];
//            });
            NSLog(@"Not found response received !");
            return;
        }
        
        expectedContentLength = [resp expectedContentLength];
        mimeType = [resp MIMEType];
    }
    else{
        NSLog(@"URLSession response object is not available !");
        return;
    }
    
    NSLog(@"Data received : %lu/%lld", [data length] + _memBuffer->GetUsedBufferSize(), expectedContentLength);
    BYTE* pData = (BYTE*)[data bytes];
    long long size = [data length];
    
    // Add received data into memory buffer and check if we need to expect more data.
    if(_memBuffer->GetUsedBufferSize() > 0 || size < expectedContentLength){
        _memBuffer->SetCurrentOffset(0);
        _memBuffer->AddBytes(pData, (unsigned int)size);
        if(_memBuffer->GetUsedBufferSize() < (int)expectedContentLength) // More data?
            return; // Wait for more data.
    }
    else // Use readonly buffer for faster performance.
        _memBuffer->SetReadonlyBuffer(pData, (unsigned int)size);
    
    Serializable* pObject = nullptr;
    bool destroyObject = false;
    
    if( _serviceCall == AuthenticateCall){
        pObject = new SessionInfo();
        if(pObject->Deserialize(_memBuffer.get())){
            dispatch_async(dispatch_get_main_queue(), ^(){
                [_delegate OnAuthenticateResponse:(SessionInfo*)pObject];
            });
        }
        else
            destroyObject = true;
    }
    
    if( _serviceCall == GetLibraryInfoCall){
        pObject = new MediaLibraryInfo();
        if(pObject->Deserialize(_memBuffer.get())){
            dispatch_async(dispatch_get_main_queue(), ^(){
                [_delegate OnLibraryInfoResponse:(MediaLibraryInfo*)pObject];
            });
        }
        else
            destroyObject = true;
    }
    
    if( _serviceCall == AudioPacketsByTimeCall || _serviceCall == AudioPacketsByOffsetCall ){
        pObject = new MediaPackets();
        if(pObject->Deserialize(_memBuffer.get())){
            if(_serviceCall == AudioPacketsByTimeCall){
                dispatch_async(dispatch_get_main_queue(), ^(){
                    [_delegate OnAudioPacketsByTimeResponse:(MediaPackets*)pObject];
                });
            }
            else{
                dispatch_async(dispatch_get_main_queue(), ^(){
                    [_delegate OnAudioPacketsByOffsetResponse:(MediaPackets*)pObject];
                });
            }
        }
        else
            destroyObject = true;
    }
    
    if( _serviceCall == GetImageResourceCall){
        unsigned int imageDataSize = _memBuffer->GetBufferSize();
        GrowableMemory* pBufferCopy = _memBuffer->CopyGrowableMemoryObject();
        dispatch_async(dispatch_get_main_queue(), ^(){
            [_delegate OnImageResourceResponse:[NSData dataWithBytes:pBufferCopy->GetBufferPtr() length:imageDataSize] MimeType:mimeType];
            delete pBufferCopy; // Destroy memory buffer object.
        });
    }

    if(pObject && destroyObject)
        delete pObject;
    _memBuffer->SetBufferSize(0); // Empty used memory buffer
}

- (void)URLSession:(NSURLSession *)session didReceiveChallenge:(NSURLAuthenticationChallenge *)challenge completionHandler:(void (^)(NSURLSessionAuthChallengeDisposition, NSURLCredential *))completionHandler{
    if([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust]){
        //if([challenge.protectionSpace.host isEqualToString:@"mydomain.com"]){
        //if([challenge.protectionSpace.host isEqualToString:@"localhost"]){
            NSURLCredential *credential = [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust];
            completionHandler(NSURLSessionAuthChallengeUseCredential,credential);
        //}
    }
}

-(void)URLSession:(NSURLSession *)session didBecomeInvalidWithError:(NSError *)error
{
//    if([session isEqual:[self backgroundSession]])
//    {
//        //self.backgroundSession = nil;
//    }
}

-(void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didResumeAtOffset:(int64_t)fileOffset expectedTotalBytes:(int64_t)expectedTotalBytes
{
    NSLog(@"resume");
}

@end
