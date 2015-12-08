//
//  ViewController.m
//  Codex.IOSExample
//
//  Created by hesy on 15/12/8.
//  Copyright © 2015年 2Coding. All rights reserved.
//

#import "ViewController.h"
#import "codec.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    NSString *instr = @"hello world";
    
    CODEC c = codec_init();
    codec_setup(c, CODECSpecialProtocol, CODECBase64);
    const CDCStream *buf = codec_encode(c, (const byte *)instr.UTF8String, instr.length);
    NSData *data = [[NSData alloc] initWithBytes:stream_data(buf) length:stream_size(buf)];
    codec_cleanup(c);
    
    NSString *outstr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    NSLog(@"\"%@\" base64 encoding is: \"%@\"", instr, outstr);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
