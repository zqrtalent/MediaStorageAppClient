//
//  ViewController.m
//  MediaStorageApp
//
//  Created by Zaqro Butskrikidze on 6/6/17.
//  Copyright © 2017 zaqro butskrikidze. All rights reserved.
//

#import "ViewController.h"
#import "CustomPresentationController.h"
#import <MediaPlayer/MediaPlayer.h>
#import <AVFoundation/AVFoundation.h>

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
}

-(void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(IBAction)onButton1:(UIButton*)sender{
 
    UIStoryboard* storyb = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
    UIViewController* modalView = [storyb instantiateViewControllerWithIdentifier:@"testView"];
    
    
    CustomPresentationController *presentationController NS_VALID_UNTIL_END_OF_SCOPE;
    presentationController = [[CustomPresentationController alloc] initWithPresentedViewController:modalView presentingViewController:self];
    modalView.transitioningDelegate = presentationController;
    
    [self presentViewController:modalView animated:YES completion:^(){
        //modalView.view.frame = CGRectMake(0.0, 0.0, 200.0, 200.0);

    }];
}


@end
