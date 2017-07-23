//
//  MainTabBarContoller.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "MainTabBarController.h"
#import "MediaStorageWebApi/MediaStreamingService.h"
#import "MediaStorageRuntimeInfo.h"

#include "Serialize/Serializable.h"
#include "Utility/GrowableMemory.h"
#include "MediaStorageWebApi/DataContracts/MediaLibraryInfo.h"

#import "ViewControllers/ArtistsViewController.h"

@interface MainTabBarContoller() <UITabBarDelegate, MediaStreamingServiceDelegate>
{
    MediaStreamingService* _service;
    std::unique_ptr<MediaLibraryInfo> _mediaLibraryInfo;
    NSString* _sessionKey;
}

@property (nonatomic, strong) UIActivityIndicatorView* refreshControl;
@property (nonatomic, strong) UINavigationController* artistsNav;
@property (nonatomic, strong) ArtistsViewController* artistsView;

@end

@implementation MainTabBarContoller

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupViews];
    
    // Do any additional setup after loading the view.
    _service = [[MediaStreamingService alloc] init:self];
    [_service Authenticate:@"zqrtalent" Password:@"77e7980a"];
    
    UIView* parentView = self.artistsView.view;
    self.refreshControl = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
    [self.refreshControl setTranslatesAutoresizingMaskIntoConstraints:NO];
    [parentView addSubview:self.refreshControl];
    
    // Y center
    [parentView addConstraint:[NSLayoutConstraint constraintWithItem:self.refreshControl attribute:NSLayoutAttributeCenterY relatedBy:NSLayoutRelationEqual toItem:parentView attribute:NSLayoutAttributeCenterY multiplier:1.0 constant:0]];
    // X center
    [parentView addConstraint:[NSLayoutConstraint constraintWithItem:self.refreshControl attribute:NSLayoutAttributeCenterX relatedBy:NSLayoutRelationEqual toItem:parentView attribute:NSLayoutAttributeCenterX multiplier:1.0 constant:0]];
    // Height
    [parentView addConstraint:[NSLayoutConstraint constraintWithItem:self.refreshControl attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0 constant:32.0]];
    // Width
    [parentView addConstraint:[NSLayoutConstraint constraintWithItem:self.refreshControl attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeNotAnAttribute multiplier:1.0 constant:32.0]];
    
    //[self.view setContentOffset:CGPointMake(0, self.tableView.contentOffset.y-self.refreshControl.frame.size.height) animated:YES];
    [self.refreshControl setHidesWhenStopped:YES];
    // Start refreshing.
    [self.refreshControl startAnimating];
    //[self.refreshControl layoutIfNeeded];
    self.refreshControl.hidden = NO;
}

-(void)setupViews{
    NSMutableArray* arrTabViews = [[NSMutableArray alloc] init];
    UIStoryboard* stBoard = [UIStoryboard storyboardWithName:@"Artists" bundle:[NSBundle mainBundle]];
    self.artistsView = (ArtistsViewController*)[stBoard instantiateViewControllerWithIdentifier:@"artistsViewId"];
    self.artistsNav  = [[UINavigationController alloc] initWithRootViewController:self.artistsView];
    [arrTabViews addObject:self.artistsNav];
    
    [self setViewControllers:arrTabViews];
    self.artistsNav.tabBarItem = [[UITabBarItem alloc]initWithTitle:@"Library" image:nil selectedImage:nil];
}

-(void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)onTouchDown{
    NSLog(@"Down");
}

#pragma mark- Tapbar delegate

//- (void)deselectTabBarItem:(UITabBar*)tabBar
//{
//    tabBar.selectedItem = nil;
//}

//- (void)tabBar:(UITabBar *)tabBar didSelectItem:(UITabBarItem *)item
//{
//    [self performSelector:@selector(deselectTabBarItem:) withObject:tabBar afterDelay:0.2];
//    
//    switch (item.tag) {
//        case 0:
//            //perform action
//            break;
//        case 1:
//            //do whatever you want to do.
//            break;
//        case 2:
//            //call method
//            break;
//        default:
//            break;
//    }
//}

#pragma mark - MediaStreamingService delegate methods

-(void)OnAuthenticateResponse:(SessionInfo *)sesInfo{
    _sessionKey = [NSString stringWithCString:sesInfo->_sessionKey.c_str() encoding:NSUTF8StringEncoding];
    [_service GetLibraryInfo:_sessionKey];
    
    // Setup streaming session.
    [MediaStorageRuntimeInfo sharedInstance].sessionInfo = [[MediaStreamingSessionInfo alloc] init:sesInfo];
    // Setup player instance object.
    [MediaStorageRuntimeInfo sharedInstance].player = [[Player alloc] init:_sessionKey];
    delete sesInfo;
}

-(void)OnLibraryInfoResponse:(MediaLibraryInfo *)libraryInfo{
    _mediaLibraryInfo = std::unique_ptr<MediaLibraryInfo>(libraryInfo);
    [[MediaStorageRuntimeInfo sharedInstance] updateMediaLibraryInfo:libraryInfo];
    
    UIViewController* currentViewController = nil;
    if([self.selectedViewController isKindOfClass:[UINavigationController class]])
        currentViewController = ((UINavigationController*)self.selectedViewController).topViewController;
    else
        currentViewController = self.selectedViewController;
    
    SEL updateDataSel = NSSelectorFromString(@"updateData");
    if([currentViewController respondsToSelector:updateDataSel])
        [currentViewController performSelector:updateDataSel];
    
    if(self.refreshControl != nil)
        [self.refreshControl stopAnimating];
}

@end
