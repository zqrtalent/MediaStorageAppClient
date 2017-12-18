//
//  MainTabBarContoller.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 6/11/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "MainTabBarController.h"
#import "ViewControllers/ArtistsViewController.h"
#import "Streaming/StreamingSession.h"
#import "MediaStorageWebApi/MediaStreamingService.h"
#include "MediaStorageWebApi/DataContracts/MediaLibraryInfo.h"
#import "MediaStorageWebApi/AuthRequest.h"
#import "MediaStorageWebApi/LibraryInfoRequest.h"

#import "AppDelegate.h"

@interface MainTabBarContoller() <UITabBarDelegate, StreamingSessionProtocol>

@property (nonatomic, strong) UIActivityIndicatorView* refreshControl;
@property (nonatomic, strong) UINavigationController* artistsNav;
@property (nonatomic, strong) ArtistsViewController* artistsView;

-(void)setupRefreshControl;
-(void)setupTabBarViews;

@end

@implementation MainTabBarContoller

-(void)viewDidLoad
{
    [super viewDidLoad];
    
    [self setupTabBarViews];
    
    [self setupRefreshControl];
    
    // Authenticate streaming session.
    StreamingSession* session = ((AppDelegate*)[UIApplication sharedApplication].delegate).streamingSession;
    session.delegate = self;
    [session authenticate:@"zack" Password:@"zackpass"];
    
    /*
    __typeof__(self) __weak weakSelf = self;
    AuthRequest* __block authReq = [[AuthRequest alloc] init:@"zack" Pass:@"zackpass" Hash:@"temphash"];
    [authReq makeRequest:^(SessionInfo* pSessInfo)
    {
        dispatch_async(dispatch_get_main_queue(), ^()
        {
            [weakSelf OnAuthenticateResponse:pSessInfo];
            [authReq cancelTasksAndInvalidate];
            authReq = nil;
        });
    }];*/
}

-(void)setupRefreshControl
{
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

-(void)setupTabBarViews
{
    NSMutableArray* arrTabViews = [[NSMutableArray alloc] init];
    UIStoryboard* stBoard = [UIStoryboard storyboardWithName:@"Artists" bundle:[NSBundle mainBundle]];
    self.artistsView = (ArtistsViewController*)[stBoard instantiateViewControllerWithIdentifier:@"artistsViewId"];
    self.artistsNav  = [[UINavigationController alloc] initWithRootViewController:self.artistsView];
    [arrTabViews addObject:self.artistsNav];
    
    [self setViewControllers:arrTabViews];
    
    self.artistsNav.tabBarItem = [[UITabBarItem alloc] initWithTitle:@"Library" image:nil selectedImage:nil];
}

#pragma mark - MediaStreamingService delegate methods

-(void)streamingSession:(StreamingSession*)sess Authenticated:(BOOL)status
{
    if(status)
    {
        // Request for library info metadata.
        [sess getAllMediaLibraryMetadata];
    }
    else
    {
        NSLog(@"Streaming session authentication has failed!");
    }
}

-(void)streamingSession:(StreamingSession*)sess AllMediaLibraryMetadata:(MediaLibraryInfo*)pInfo
{
    assert(pInfo);
    MediaLibraryInfo* pMlInfo = new MediaLibraryInfo();
    pInfo->Copy(pMlInfo);
    
    // Keep media library info metadata object.
    [((AppDelegate*)[UIApplication sharedApplication].delegate) setMediaLibraryInfo:pMlInfo];
    
    dispatch_async(dispatch_get_main_queue(), ^()
    {
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
    });
}

@end
