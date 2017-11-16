//
//  NowPlayingController.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/6/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface NowPlayingController : UIViewController

@property (nonatomic, weak) IBOutlet UIImageView* artworkImageView;
@property (nonatomic, weak) IBOutlet UISlider* playbackTrackbar;
@property (nonatomic, weak) IBOutlet UILabel* currTimeSecLabel;
@property (nonatomic, weak) IBOutlet UILabel* currTimeSecLeftLabel;
@property (nonatomic, weak) IBOutlet UILabel* songNameLabel;
@property (nonatomic, weak) IBOutlet UILabel* artistAlbumLabel;
@property (nonatomic, weak) IBOutlet UIButton* prevButton;
@property (nonatomic, weak) IBOutlet UIButton* playPauseButton;
@property (nonatomic, weak) IBOutlet UIButton* nextButton;
@property (nonatomic, weak) IBOutlet UISlider* volumeBar;

-(IBAction)onCloseModal;
-(IBAction)onPlayPause;
-(IBAction)onNext;
-(IBAction)onPrev;
-(IBAction)onVolumeChanged:(UISlider*)sender;
-(IBAction)onPlaybackTrackbarValueChanged:(UISlider*)sender;

@end
