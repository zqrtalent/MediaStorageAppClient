//
//  NowPlayingController.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/6/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "NowPlayingController.h"
#import <MediaPlayer/MediaPlayer.h>

#import "../MediaStorageWebApi/ImageResourceRequest.h"
#import "../MediaStorageWebApi/MediaStreamingService.h"
#import "../StreamPlayer/StreamPlayerManager.h"
#import "../StreamPlayer/Player.h"
#import "../AppDelegate.h"


@interface NowPlayingController()<MediaStreamingServiceDelegate, PlayerDelegate>
{
    NSString* _sessionKey;
    int _currentOffset;
    int _currentMSec;
    
    BOOL _trackbarIsCaptured;
    float _currentPlayTimeSecFloat;
    int _currentPlayTimeSec;
}

//@property (nonatomic, assign) BOOL _trackbarIsCaptured;

@property (nonatomic, strong) UIImage* artworkImage;
@property (nonatomic, strong) MediaStreamingService* service;
@property (nonatomic, assign) id<PlayerDelegate> addinDelegate;

@end

@implementation NowPlayingController

-(void)viewDidLoad
{
    [super viewDidLoad];
    
    StreamPlayerManager* playerMan =  [AppDelegate sharedInstance].playerManager;
    // Register delegate for receiving events.
    [playerMan.player setDelegate:self];
    
    // Use AppDelegate instance as additioal player delegate.
    id appDel = [UIApplication sharedApplication].delegate;
    if([appDel conformsToProtocol:@protocol(PlayerDelegate)])
        self.addinDelegate = appDel;
    
    // Update UI.
    [self updateNowPlayingUI:YES WithTimingInfo:YES];
    
    // Play song.
    //    if(info.NowPlaying == nil || [info.NowPlaying.mediaId compare:self.media.mediaId options:NSCaseInsensitiveSearch] != NSOrderedSame ){
    //        /*Stops if already playing*/
    //        [self.player play:self.media.mediaId At:0/*Play at second*/];
    //    }
    
    //[self.player play:playInfo.mediaId At:playInfo->_playAtMSec];
    
    //    // Do any additional setup after loading the view.
    //    [self updateUIBasedOnInfo];
    //    [self initStreamingService];
    //
    //    // Request for image resource.
    //    if(self.media.artworkImageId != nil && [self.media.artworkImageId length] > 0 )
    //        [self.service GetImageResource:_sessionKey ImageId:self.media.artworkImageId SizeType:@"medium"];
}

-(void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    
    // Set player delegate.
    [[AppDelegate sharedInstance].playerManager.player setDelegate:self.addinDelegate];
    
    self.service = nil;
    
    // Dispose
    //self.artworkImage = nil;
    //self.artworkImageView = nil;
}

-(void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(IBAction)onCloseModal
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

-(IBAction)onPlayPause
{
   [[AppDelegate sharedInstance].playerManager playPauseToggle];
}

-(IBAction)onNext
{
    [[AppDelegate sharedInstance].playerManager playNext];
}

-(IBAction)onPrev
{
    [[AppDelegate sharedInstance].playerManager playPrev];
}

-(IBAction)onVolumeChanged:(UISlider*)sender
{
    [[AppDelegate sharedInstance].playerManager.player setVolume:sender.value];
}

-(IBAction)onPlaybackTrackbarValueChanged:(UISlider*)sender
{
    float seekTimeSec = [sender value];
    _trackbarIsCaptured = NO;
    [[AppDelegate sharedInstance].playerManager.player seekAtTime:seekTimeSec];
}

-(IBAction)onPlaybackTrackbarTouchDown:(UISlider*)sender
{
    _trackbarIsCaptured = YES;
}

-(IBAction)onPlaybackTrackbarTouchUpInside:(UISlider*)sender
{
    _trackbarIsCaptured = NO;
}

-(IBAction)onPlaybackTrackbarTouchUpOutside:(UISlider*)sender
{
    _trackbarIsCaptured = NO;
}

#pragma mark - MediaStreamingServiceDelegate methods

-(void)OnImageResourceResponse:(NSData*)imageData MimeType:(NSString*)mimeType
{
    auto image = [UIImage imageWithData:imageData];
    if(image != nil){
        [self.artworkImageView setImage:image];
        self.artworkImage = image;
    }
}

-(void)updateNowPlayingUI:(BOOL)updateMetadataInfo WithTimingInfo:(BOOL)updateTimingInfo
{
    StreamPlayerManager* playerMan =  [AppDelegate sharedInstance].playerManager;
    AudioPlayerState playerState = [playerMan.player getPlayerState];
    AudioMetadataInfo* nowPlayingInfo = playerMan.player.audioMetadata;
    
    // Disable play/pause button when buffering is in action.
    [self.playPauseButton setEnabled:!(playerState == AudioPlayer_Buffering)];
    
    // Update volume bar.
    self.volumeBar.value = [playerMan.player getVolume];
    
    // Play/Pause button.
    [self.playPauseButton setTitle: playerState == AudioPlayer_Playing ? @"Pause" : @"Play" forState:UIControlStateNormal];
    
    int currPlayingTime = (int)playerMan.player.CurrentTimeInSec;
    if(updateMetadataInfo)
    {
        self.songNameLabel.text = nowPlayingInfo.songName;
        self.artistAlbumLabel.text = [NSString stringWithFormat:@"%@ - %@", nowPlayingInfo.artistName, nowPlayingInfo.albumName];
    }
    
    if(updateTimingInfo)
    {
        if(playerState == AudioPlayer_Stopped) // Ended
            currPlayingTime = 0;
        
        int secLeft = (((int)nowPlayingInfo.durationSec) - currPlayingTime);
        self.currTimeSecLabel.text = [NSString stringWithFormat:@"%d:%02d", currPlayingTime/60, currPlayingTime%60 ];
        self.currTimeSecLeftLabel.text = [NSString stringWithFormat:@"-%d:%02d", secLeft/60, secLeft%60];
        
        // Setup playback trackbar values and position.
        [self.playbackTrackbar setMinimumValue:0.0];
        [self.playbackTrackbar setMaximumValue:nowPlayingInfo.durationSec];
        [self.playbackTrackbar setValue:(float)(currPlayingTime) animated:NO];
    }
}

-(void)initiateUI
{
    [self updateNowPlayingUI:YES WithTimingInfo:YES];
    
    AudioMetadataInfo* nowPlaying = [AppDelegate sharedInstance].playerManager.nowPlaying;
    // Request for image resource.
    if(nowPlaying.artworkId != nil && [nowPlaying.artworkId length] > 0 )
    {
        NSString* sessionId =  [AppDelegate sharedInstance].streamingSession.sessionId;
        __typeof__(self) __weak weakSelf = self;
        ImageResourceRequest* __block req = [[ImageResourceRequest alloc] init:sessionId ImageId:nowPlaying.artworkId SizeType:@"medium"];
        [req makeRequest:^(NSData* imageData)
        {
            dispatch_async(dispatch_get_main_queue(), ^()
            {
                [weakSelf OnImageResourceResponse:imageData MimeType:nil];
                [req cancelTasksAndInvalidate];
                req = nil;
            });
        }];
        
        /*
        if(self.service == nil)
        {
            [self initStreamingService];
        }
        [self.service GetImageResource:_sessionKey ImageId:nowPlaying.artworkImageId SizeType:@"medium"];
        */
    }

}

#pragma mark - PlayerDelegate methods

-(void)onPlayStarted:(BOOL)resumed
{
    NSLog(@"onPlayStarted");
    
    dispatch_async(dispatch_get_main_queue(), ^{
        //[self.playPauseButton setTitle:@"Pause" forState:UIControlStateNormal];
        if(!resumed){
            [self initiateUI];
        }
        else{
            [self updateNowPlayingUI:NO WithTimingInfo:NO];
        }
        
        [self.addinDelegate onPlayStarted:resumed];
    });
}

-(void)onPlayEnded
{
    dispatch_async(dispatch_get_main_queue(), ^(){
        [self updateNowPlayingUI:YES WithTimingInfo:YES];
        [self.addinDelegate onPlayEnded];
    });
}

-(void)onPaused
{
    dispatch_async(dispatch_get_main_queue(), ^(){
        [self updateNowPlayingUI:NO WithTimingInfo:NO];
        [self.addinDelegate onPaused];
    });
}

-(void)onBufferingStarted
{
    dispatch_async(dispatch_get_main_queue(), ^(){
        NSLog(@"onBufferingStarted");
        [self updateNowPlayingUI:NO WithTimingInfo:NO];
        [self.addinDelegate onBufferingStarted];
    });
}

-(void)onBufferingEnded
{
    dispatch_async(dispatch_get_main_queue(), ^(){
        NSLog(@"onBufferingEnded");
        [self updateNowPlayingUI:NO WithTimingInfo:NO];
        [self.addinDelegate onBufferingEnded];
    });
}

-(void)onPlayTimeUpdate:(unsigned int)msec
{
    int currentTimeSec = (int)(msec/1000.0);
    _currentPlayTimeSecFloat = (msec/1000.0);
    if(currentTimeSec == _currentPlayTimeSec && _trackbarIsCaptured)
        return;
    
    __typeof__(self) __weak weakSelf = self;
    dispatch_async(dispatch_get_main_queue(), ^{
        //_currentPlayTimeSecFloat = (msec/1000.0);
        //int currentTimeSec = (int)_currentPlayTimeSecFloat;
        if(currentTimeSec != _currentPlayTimeSec)
        {
            _currentPlayTimeSec = currentTimeSec;
            if(!_trackbarIsCaptured)
                [weakSelf.playbackTrackbar setValue:_currentPlayTimeSecFloat];
            
            // Update track bar and playback time labels.
            float timeLeftSec = [AppDelegate sharedInstance].playerManager.nowPlaying.durationSec - _currentPlayTimeSecFloat;
            
            // Update playback times.
            self.currTimeSecLabel.text = [NSString stringWithFormat:@"%d:%02d", currentTimeSec/60, currentTimeSec%60];
            self.currTimeSecLeftLabel.text = [NSString stringWithFormat:@"-%d:%02d", (int)(timeLeftSec/60), (int)(timeLeftSec)%60];
            
            [self.addinDelegate onPlayTimeUpdate:msec];
        }
        else
        {
            if(!_trackbarIsCaptured)
                [weakSelf.playbackTrackbar setValue:_currentPlayTimeSecFloat];
        }
    });
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
