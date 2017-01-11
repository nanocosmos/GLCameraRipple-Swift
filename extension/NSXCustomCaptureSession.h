//
//  NSXCustomCaptureSession.h
//  FPVDemo
//
//  Created by Oliver Arnold on 25/05/16.
//
//  @copyright (c) 2016 nanocosmos. All rights reserved.
//  http://www.nanocosmos.de

#import <AVFoundation/AVFoundation.h>

@interface NSXCustomCaptureSession : AVCaptureSession

@property (nonatomic, strong) AVCaptureVideoDataOutput* myVideoOutput;
@property (nonatomic, strong) AVCaptureAudioDataOutput* myAudioOutput;

-(id) init;

-(void) addInput:(AVCaptureInput *)input;
-(void) addOutput:(AVCaptureOutput *)output;

-(void) startRunning;
-(void) stopRunning;

-(void) supplyCMSampleBufferRef:(CVImageBufferRef)buffer;

@end
