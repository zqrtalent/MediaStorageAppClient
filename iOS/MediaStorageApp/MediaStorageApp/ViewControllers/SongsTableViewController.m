//
//  SongsTableViewController.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "SongsTableViewController.h"
#import "NowPlayingController.h"

#import "../CustomPresentationController.h"
#import "../MediaStream/MediaInfo.h"
#import "../MediaStorageRuntimeInfo.h"

@interface SongsTableViewController (){
    MLAlbum* _album;
    MLArtist* _artist;
}

@end

@implementation SongsTableViewController

-(void)setData:(MLAlbum*)album Artist:(MLArtist*)artist{
    _album = album;
    _artist = artist;
    [self.tableView reloadData];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    if(_album != nullptr)
        self.navigationItem.backBarButtonItem.title = [NSString stringWithUTF8String:_album->_name.c_str()];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return _album ? _album->_songs.GetCount() : 0;
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"cell0" forIndexPath:indexPath];
    auto song = _album->_songs.GetAt((int)indexPath.row);
    [cell textLabel].text = [NSString stringWithUTF8String:song->_name.c_str()];
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    UIStoryboard* storyBoard = [UIStoryboard storyboardWithName:@"NowPlaying" bundle:nil];
    NowPlayingController* nowPlaying = (NowPlayingController*)[storyBoard instantiateViewControllerWithIdentifier:@"nowPlayingViewId"];
    MLSong* song = _album->_songs.GetAt((int)indexPath.row);
    
    // Schedule media to play.
    MediaStorageRuntimeInfo* info = [MediaStorageRuntimeInfo sharedInstance];
    if(info.NowPlaying == nil || [info.NowPlaying.mediaId compare:[NSString stringWithUTF8String:song->_id.c_str()] options:NSCaseInsensitiveSearch] != NSOrderedSame)
    {
        [info scheduleMedia:[NSString stringWithUTF8String:song->_id.c_str()] FromArtist:[NSString stringWithUTF8String:_artist->_id.c_str()] AndFromAlbum:[NSString stringWithUTF8String:_album->_id.c_str()] PlayInstantly:YES];
    }
    
    [nowPlaying setModalTransitionStyle:UIModalTransitionStyleCoverVertical];
    [nowPlaying setModalPresentationStyle:UIModalPresentationPopover];
    
    
//    CustomPresentationController *presentationController NS_VALID_UNTIL_END_OF_SCOPE;
//    presentationController = [[CustomPresentationController alloc] initWithPresentedViewController:nowPlaying presentingViewController:self];
//    nowPlaying.transitioningDelegate = presentationController;
//    
//    [self presentViewController:nowPlaying animated:YES completion:^(){
//        //modalView.view.frame = CGRectMake(0.0, 0.0, 200.0, 200.0);
//    }];
    
    [self presentViewController:nowPlaying animated:YES completion:^(){
        //nowPlaying.view.frame = CGRectMake(0, 0, 200, 200);
        //nowPlaying.view.center = self.view.superview.center;
        //NSLog(@"Completed");
    }];
}

@end
