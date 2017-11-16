//
//  AlbumsViewController.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <UIKit/UIKit.h>
class MLArtist;

@interface AlbumsViewController : UITableViewController

-(void)setData:(MLArtist*)artist;
@end
