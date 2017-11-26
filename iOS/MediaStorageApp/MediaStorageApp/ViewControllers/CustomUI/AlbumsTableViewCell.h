//
//  AlbumsTableViewCell.h
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 11/26/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface AlbumsTableViewCell : UITableViewCell

@property (nonatomic, weak) IBOutlet UILabel* albumName;
@property (nonatomic, weak) IBOutlet UILabel* genre;
@property (nonatomic, weak) IBOutlet UILabel* year;

@end
