//
//  ArtistsViewController.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/3/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "ArtistsViewController.h"
#import "AlbumsViewController.h"
#import "../MediaStorageRuntimeInfo.h"
#include "../MediaStorageWebApi/DataContracts/MediaLibraryInfo.h"

@interface ArtistsViewController () <UITableViewDelegate, UITableViewDataSource>
{
    MediaLibraryInfo* _mediaLibraryInfo;
}

@end

@implementation ArtistsViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    _mediaLibraryInfo = nullptr;
    self.navigationItem.title = @"Artists";
    
//    // Do any additional setup after loading the view.
//    _service = [[MediaStreamingService alloc] init:self];
//    [_service Authenticate:@"zqrtalent" Password:@"77e7980a"];
//
//    auto refreshControl = [[UIRefreshControl alloc] init];
//    
//    refreshControl.attributedTitle = [[NSAttributedString alloc]initWithString:@"Loading data..."];
//    [refreshControl addTarget:self action:@selector(refreshMediaLibrary:) forControlEvents:UIControlEventValueChanged];
//    [self.tableView setRefreshControl:refreshControl];
//    [self.tableView setContentOffset:CGPointMake(0, self.tableView.contentOffset.y-self.refreshControl.frame.size.height) animated:YES];
//    
//    // Start refreshing.
//    [refreshControl beginRefreshing];
//    //[refreshControl layoutIfNeeded];
//    refreshControl.hidden = NO;
}

-(void)updateData{
    _mediaLibraryInfo = [MediaStorageRuntimeInfo sharedInstance]->_mlInfo;
    [self.tableView reloadData];
}

-(void)refreshMediaLibrary:(UIRefreshControl*)refresh{
    // Start refreshing the data.
    //[_service GetLibraryInfo:_sessionKey];
}
    
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    UIStoryboard* stBoard = [UIStoryboard storyboardWithName:@"Albums" bundle:[NSBundle mainBundle]];
    AlbumsViewController* viewCtrl = [stBoard instantiateViewControllerWithIdentifier:@"albumsViewId"];
    if(viewCtrl != nil){
        int artistIndex = (int)indexPath.row;
        auto artist = _mediaLibraryInfo->_artists.GetAt(artistIndex);
        [viewCtrl setData:artist];
        [self.navigationController pushViewController:viewCtrl animated:YES];
    }
}

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return 1;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return _mediaLibraryInfo ? _mediaLibraryInfo->_artists.GetCount() : 0;
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:@"cell0"];
    [cell textLabel].text = [NSString stringWithUTF8String:_mediaLibraryInfo->_artists.GetAt((int)indexPath.row)->_name.c_str()];
    return cell;
}

@end
