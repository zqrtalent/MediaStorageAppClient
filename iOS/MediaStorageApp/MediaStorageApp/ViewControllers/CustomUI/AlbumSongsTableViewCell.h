//
//  AlbumSongTableViewCell.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 11/26/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface AlbumSongsTableViewCell : UITableViewCell

@property (nonatomic, weak) IBOutlet UILabel* songName;
@property (nonatomic, weak) IBOutlet UILabel* duration;

@end
