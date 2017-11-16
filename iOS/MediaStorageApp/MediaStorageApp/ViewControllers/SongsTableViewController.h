//
//  SongsTableViewController.h
//  Mp3PlayerDemo
//
//  Created by Zaqro Butskrikidze on 5/4/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <UIKit/UIKit.h>
class MLArtist;
class MLAlbum;

@interface SongsTableViewController : UITableViewController

-(void)setData:(MLAlbum*)album Artist:(MLArtist*)artist;

@end
