//
//  ApiRequestBase.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 9/30/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "ApiRequestBase.h"
#include "Utility/GrowableMemory.h"
#include "Serialize/Serializable.h"

@interface ApiRequestBase()<NSURLSessionDelegate, NSURLSessionDataDelegate>
{
    ResponseCallback _callback;
}

@property (nonatomic, strong) NSURLSessionTask* sessionTask;
@property (nonatomic, strong) NSURLSession* session;
@property (nonatomic, strong) NSMutableData* receivedData;
@property (nonatomic, assign) NSUInteger receivedDataSizeUsed;

-(Serializable*)createResponseBodyObject;

@end

@implementation ApiRequestBase

-(instancetype)init
{
    return [super init];
}

-(void)cancelTasksAndInvalidate
{
    if(self.session)
    {
        [self.session invalidateAndCancel];
        self.receivedData = nil;
        self.session = nil;
    }
}

-(void)finishTasksAndInvalidate
{
    if(self.session)
    {
        [self.session finishTasksAndInvalidate];
        self.receivedData = nil;
        self.session = nil;
    }
}

-(void)httpGet:(NSString*)urlPathWithQuery WithCallback:(ResponseCallback)respCallback;
{
    _callback = respCallback;
    NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"%@%@", self.host, urlPathWithQuery]];
    
    if(self.session == nil)
    {
        self.session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration ephemeralSessionConfiguration]
                                                     delegate:self delegateQueue:nil];
    }
    
    self.sessionTask = [self.session dataTaskWithURL:url];
    [self.sessionTask resume];    
}

-(Serializable*)createResponseBodyObject
{
    return nil; // Need to be defined explicitly in child class.
}

#pragma mark - DataTask callback methods.

-(void)responseArrivedCallback:(void*)decodedBodyData WithData:(NSData*)data WithErrorString:(NSString*)errorString
{
    // No action
}

-(void)dataReceiveCompleted:(NSData*)data ContentType:(NSString*)contentMimeType
{
    Serializable* pObject = [self createResponseBodyObject];
    if(pObject != nullptr)
    {
        GrowableMemory mem(0, 0, false);
        mem.SetReadonlyBuffer((BYTE*)data.bytes, (int)data.length);
        
        if(pObject->Deserialize(&mem))
        {
            //[self responseArrivedCallback: pObject WithData:data WithErrorString:nil];
            if(_callback)
                (_callback)(pObject, data, nil);
        }
        else
        {
//            mem.SetCurrentOffset(0);
//            pObject->Deserialize(&mem);
            //[self responseArrivedCallback: nullptr WithData:data WithErrorString:@"Data deserialization error!"];
            if(_callback)
                (_callback)(nullptr, data, @"Data deserialization error!");
            
            // Destroy object.
            delete pObject;
        }
    }
    else
    {
        if(_callback)
            (_callback)(nullptr, data, nil);
    }
}

-(void)dataReceiveCompletedWithError:(NSString*)errorDescription
{
    //[self responseArrivedCallback: nullptr WithData:nil WithErrorString:errorDescription];
    if(_callback)
        (_callback)(nullptr, nil, errorDescription);
}

#pragma mark - NSURLSessionDelegate

-(void)URLSession:(NSURLSession *)session didReceiveChallenge:(NSURLAuthenticationChallenge *)challenge completionHandler:(void (^)(NSURLSessionAuthChallengeDisposition, NSURLCredential *))completionHandler
{
    if([challenge.protectionSpace.authenticationMethod isEqualToString:NSURLAuthenticationMethodServerTrust])
    {
        //if([challenge.protectionSpace.host isEqualToString:@"mydomain.com"]){
        //if([challenge.protectionSpace.host isEqualToString:@"localhost"]){
        NSURLCredential *credential = [NSURLCredential credentialForTrust:challenge.protectionSpace.serverTrust];
        completionHandler(NSURLSessionAuthChallengeUseCredential,credential);
        //}
    }
}
/*
-(void)URLSession:(NSURLSession *)session didBecomeInvalidWithError:(NSError *)error
{
    self.session = nil;
}

#pragma mark - any task completion.

-(void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error
{
    if(error)
        NSLog(@"Session task completed with error %@", error);
    else
        NSLog(@"Session task completed!");
}*/

#pragma mark - NSURLSessionDataDelegate
/*
-(void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask willCacheResponse:(NSCachedURLResponse *)proposedResponse completionHandler:(void (^)(NSCachedURLResponse * _Nullable))completionHandler
{
    completionHandler(proposedResponse); // Default caching response.
}
*/
-(void)URLSession:(NSURLSession *)session dataTask:(NSURLSessionDataTask *)dataTask didReceiveData:(NSData *)data
{
    long long expectedContentLength = 0;
    NSString* mimeType = nil;
    auto resp = (NSHTTPURLResponse*)[dataTask response];
    
    if(resp != nil)
    {
        if(resp.statusCode != 200)
        {
            if(resp.statusCode == 404)
                [self dataReceiveCompletedWithError:@"Not found response received !"];
            else
                [self dataReceiveCompletedWithError:[NSString stringWithFormat:@"Http response with status code %ld", (long)resp.statusCode]];
            return;
        }
        
        expectedContentLength = [resp expectedContentLength];
        mimeType = [resp MIMEType];
    }
    else
    {
        [self dataReceiveCompletedWithError:@"URLSession response object is not available !"];
        return;
    }
    
    BYTE* pData = (BYTE*)[data bytes];
    long long size = [data length];
    
    // Add received data into memory buffer and check if we need to expect more data.
    if((self.receivedData != nil && self.receivedData.length > 0) || size < expectedContentLength)
    {
        if(self.receivedData == nil)
        {
            self.receivedData = [NSMutableData dataWithBytes:pData length:size];
            self.receivedDataSizeUsed = size;
        }
        else
        {
            self.receivedDataSizeUsed += size;
            if(self.receivedDataSizeUsed > self.receivedData.length)
            {
                [self.receivedData increaseLengthBy: (self.receivedDataSizeUsed - self.receivedData.length)];
            }
            
            [self.receivedData replaceBytesInRange:NSMakeRange((self.receivedDataSizeUsed - size), size) withBytes:pData];
        }
        
        // More data?
        if(self.receivedDataSizeUsed < expectedContentLength)
            return; // Wait for more data.
        
        [self dataReceiveCompleted:self.receivedData ContentType:mimeType];
        self.receivedDataSizeUsed = 0;
        
    }
    else
        [self dataReceiveCompleted:data ContentType:mimeType];
}

@end

