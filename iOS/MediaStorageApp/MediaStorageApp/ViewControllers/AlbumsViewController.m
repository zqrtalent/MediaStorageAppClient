//
//  AlbumsViewController.m
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "AlbumsViewController.h"
#import "SongsTableViewController.h"

@interface AlbumsViewController () <UITableViewDelegate, UITableViewDataSource>
{
    MLArtist* _artist;
}

@end

@implementation AlbumsViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    if(_artist){
        self.navigationItem.title = [NSString stringWithUTF8String:_artist->_name.c_str()];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)setData:(MLArtist*)artist{
    _artist = artist;
    [self.tableView reloadData];
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    UIStoryboard* stBoard = [UIStoryboard storyboardWithName:@"Songs" bundle:[NSBundle mainBundle]];
    SongsTableViewController* viewCtrl = [stBoard instantiateViewControllerWithIdentifier:@"songsViewId"];
    if(viewCtrl != nil){
        int albumIndex = (int)indexPath.row;
        auto album = _artist->_albums.GetAt(albumIndex);
        [viewCtrl setData:album Artist:_artist];
        [self.navigationController pushViewController:viewCtrl animated:YES];
    }
}

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView{
    return 1;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section{
    return _artist ? _artist->_albums.GetCount() : 0;
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath{
    UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:@"cell0"];
    MLAlbum* album = _artist->_albums.GetAt((int)indexPath.row);
    if(album->_year > 0){
        [cell textLabel].text = [NSString stringWithFormat:@"%s - %i", album->_name.c_str(), album->_year];
    }
    else{
        [cell textLabel].text = [NSString stringWithUTF8String: album->_name.c_str()];
    }
    return cell;
}

@end
