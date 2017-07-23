//
//  NowPlayingController.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/6/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "NowPlayingController.h"
#import <MediaPlayer/MediaPlayer.h>
#import "../MediaStorageWebApi/MediaStreamingService.h"
#import "../Player/Player.h"
#import "MediaStorageRuntimeInfo.h"


@interface NowPlayingController()<MediaStreamingServiceDelegate, PlayerDelegate>
{
    NSString* _sessionKey;
    int _currentOffset;
    int _currentMSec;
    
    BOOL _trackbarIsCaptured;
    float _currentPlayTimeSecFloat;
    int _currentPlayTimeSec;
}

@property (nonatomic, strong) UIImage* artworkImage;
@property (nonatomic, strong) Player* player;
@property (nonatomic, strong) MediaStreamingService* service;
@property (nonatomic, strong) MediaInfo* media;
@property (nonatomic, assign) id<PlayerDelegate> addinDelegate;

@end

@implementation NowPlayingController

-(void)updateNowPlayingUI:(BOOL)updateMetadataInfo WithTimingInfo:(BOOL)updateTimingInfo {
    AudioPlayerState playerState = [self.player getPlayerState];
    
    // Disable play/pause button when buffering is in action.
    [self.playPauseButton setEnabled:!(playerState == AudioPlayer_Buffering)];
    // Play/Pause button.
    [self.playPauseButton setTitle: playerState == AudioPlayer_Playing ? @"Pause" : @"Play" forState:UIControlStateNormal];
    
    int currPlayingTime = (int)self.player.CurrentTimeInSec;
    if(updateMetadataInfo){
        self.songNameLabel.text = self.media.songName;
        self.artistAlbumLabel.text = [NSString stringWithFormat:@"%@ - %@", self.media.artist, self.media.album];
    }
    
    if(updateTimingInfo){
        if(playerState == AudioPlayer_Stopped) // Ended
            currPlayingTime = 0;
        
        int secLeft = (((int)self.media.DurationInSec) - currPlayingTime);
        self.currTimeSecLabel.text = [NSString stringWithFormat:@"%d:%02d", currPlayingTime/60, currPlayingTime%60 ];
        self.currTimeSecLeftLabel.text = [NSString stringWithFormat:@"-%d:%02d", secLeft/60, secLeft%60];
        
        // Setup playback trackbar values and position.
        [self.playbackTrackbar setMinimumValue:0.0];
        [self.playbackTrackbar setMaximumValue:self.media.DurationInSec];
        [self.playbackTrackbar setValue:(float)(currPlayingTime) animated:NO];
    }
}

-(void)initStreamingService{
    _sessionKey = [MediaStorageRuntimeInfo sharedInstance].sessionInfo.SessionKey;
    _service = [[MediaStreamingService alloc] init:self];
}

-(void)viewDidLoad {
    [super viewDidLoad];
    
    MediaStorageRuntimeInfo *info = [MediaStorageRuntimeInfo sharedInstance];
    
    // Retrieve player instance and register delegate for receiving events.
    self.player = info.player;
    [self.player setDelegate:self];
    
    // Use AppDelegate instance as additioal player delegate.
    id appDel = [UIApplication sharedApplication].delegate;
    if([appDel conformsToProtocol:@protocol(PlayerDelegate)])
        self.addinDelegate = appDel;
    
    [self initiateUIForPlay];
    
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

-(void)viewDidDisappear:(BOOL)animated{
    [super viewDidDisappear:animated];
    
    // Set player delegate.
    [self.player setDelegate:self.addinDelegate];
    
    self.media = nil;
    self.player = nil;
    self.service = nil;
    
    // Dispose
    //self.artworkImage = nil;
    //self.artworkImageView = nil;
}

-(void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(IBAction)onCloseModal{
    [self dismissViewControllerAnimated:YES completion:nil];
}

-(IBAction)onPlayPause{
    // Start play/resume
    if([self.player getPlayerState] == AudioPlayer_Stopped || [_player getPlayerState] == AudioPlayer_Paused)
        [self.player play:self.media.mediaId At:-1];
    else
        [self.player pause: nil];
}

-(IBAction)onNext{
    [[MediaStorageRuntimeInfo sharedInstance] playNext];
}

-(IBAction)onPrev{
    [[MediaStorageRuntimeInfo sharedInstance] playPrev];
}

-(IBAction)onVolumeChanged:(UISlider*)sender{
    [self.player setVolume:sender.value];
}

-(IBAction)onPlaybackTrackbarValueChanged:(UISlider*)sender{
    float seekTimeSec = [sender value];
    _trackbarIsCaptured = NO;
    [self.player seekAtTime:seekTimeSec];
}

-(IBAction)onPlaybackTrackbarTouchDown:(UISlider*)sender{
    _trackbarIsCaptured = YES;
}

-(IBAction)onPlaybackTrackbarTouchUpInside:(UISlider*)sender{
    _trackbarIsCaptured = NO;
}

-(IBAction)onPlaybackTrackbarTouchUpOutside:(UISlider*)sender{
    _trackbarIsCaptured = NO;
}

-(void)OnImageResourceResponse:(NSData*)imageData MimeType:(NSString*)mimeType{
    auto image = [UIImage imageWithData:imageData];
    if(image != nil){
        [self.artworkImageView setImage:image];
        self.artworkImage = image;
    }
}

-(void)initiateUIForPlay{
    MediaInfo* nowPlaying = [MediaStorageRuntimeInfo sharedInstance].NowPlaying;
    // Keep now playing media.
    self.media = nowPlaying;
    // Do any additional setup after loading the view.
    [self updateNowPlayingUI:YES WithTimingInfo:YES];
}

-(void)onPlayStarted:(BOOL)resumed{
    NSLog(@"onPlayStarted");
    
    dispatch_async(dispatch_get_main_queue(), ^(){
        //[self.playPauseButton setTitle:@"Pause" forState:UIControlStateNormal];
        if(!resumed){
            [self initiateUIForPlay];
        }
        else{
            [self updateNowPlayingUI:NO WithTimingInfo:NO];
        }
        
        [self.addinDelegate onPlayStarted:resumed];
    });
}

-(void)onPlayEnded{
    dispatch_async(dispatch_get_main_queue(), ^(){
        [self updateNowPlayingUI:YES WithTimingInfo:YES];
        [self.addinDelegate onPlayEnded];
    });
}

-(void)onPaused{
    dispatch_async(dispatch_get_main_queue(), ^(){
        [self updateNowPlayingUI:NO WithTimingInfo:NO];
        [self.addinDelegate onPaused];
    });
}

-(void)onBufferingStarted{
    dispatch_async(dispatch_get_main_queue(), ^(){
        NSLog(@"onBufferingStarted");
        [self updateNowPlayingUI:NO WithTimingInfo:NO];
        [self.addinDelegate onBufferingStarted];
    });
}

-(void)onBufferingEnded{
    dispatch_async(dispatch_get_main_queue(), ^(){
        NSLog(@"onBufferingEnded");
        [self updateNowPlayingUI:NO WithTimingInfo:NO];
        [self.addinDelegate onBufferingEnded];
    });
}

-(void)onPlayTimeUpdate:(unsigned int)msec{
    dispatch_async(dispatch_get_main_queue(), ^(){
        _currentPlayTimeSecFloat = (msec/1000.0);
        int currentTimeSec = (int)_currentPlayTimeSecFloat;
        if(currentTimeSec != _currentPlayTimeSec){
            _currentPlayTimeSec = currentTimeSec;
            if(!_trackbarIsCaptured)
                [self.playbackTrackbar setValue:_currentPlayTimeSecFloat];
            
            // Update track bar and playback time labels.
            //float timeLeftSec = self.player.DurationInSec - _currentPlayTimeSecFloat;
            float timeLeftSec = self.media.DurationInSec - _currentPlayTimeSecFloat;
            
            // Update playback times.
            self.currTimeSecLabel.text = [NSString stringWithFormat:@"%d:%02d", currentTimeSec/60, currentTimeSec%60];
            self.currTimeSecLeftLabel.text = [NSString stringWithFormat:@"-%d:%02d", (int)(timeLeftSec/60), (int)(timeLeftSec)%60];
            
            [self.addinDelegate onPlayTimeUpdate:msec];
        }
        else{
            if(!_trackbarIsCaptured)
                [self.playbackTrackbar setValue:_currentPlayTimeSecFloat];
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
