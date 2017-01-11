//
//  NSXCustomCaptureSession.m
//
//  Created by Oliver Arnold on 25/05/16.
//
//  @copyright (c) 2016 nanocosmos. All rights reserved.
//  http://www.nanocosmos.de

#import "NSXCustomCaptureSession.h"
#import "NSXTimer.h"

@interface NSXCustomCaptureSession ()

@property (nonatomic, strong) NSXTimer* timer;
@property (nonatomic, assign) int64_t firstTimestamp;
@property (nonatomic, assign) int64_t lastTimestamp;

@end

@implementation NSXCustomCaptureSession

-(id) init
{
    self = [super init];
    
    if(self)
    {
        _timer = [[NSXTimer alloc] init];
        _firstTimestamp = INT64_MAX;
        _lastTimestamp = 0;
    }
    
    return self;
}

-(void) addInput:(AVCaptureInput *)input
{
    [super addInput:input];
}

-(void) addOutput:(AVCaptureOutput *)output
{
    if ([output isKindOfClass:[AVCaptureVideoDataOutput class]]) {
        self.myVideoOutput = (AVCaptureVideoDataOutput*)output;
    }else if([output isKindOfClass:[AVCaptureAudioDataOutput class]])
    {
        //self.myAudioOutput = (AVCaptureAudioDataOutput*)output; // if you want to use a custom audio capture device
        [super addOutput: output]; // uses the standard microphone of the iOS device
    }
}

-(void) startRunning
{
    [super startRunning];
}

-(void) stopRunning
{
    [super stopRunning];
}

- (void) decompressedFrame: (CVImageBufferRef) image
{
    [self supplyCMSampleBufferRef: image];
}

#define FourCC2Str(code) (char[5]){(code >> 24) & 0xFF, (code >> 16) & 0xFF, (code >> 8) & 0xFF, code & 0xFF, 0}

// this method has to be called periodically - e.g. with CADisplayLink
-(void) supplyCMSampleBufferRef:(CVImageBufferRef)buffer
{
    //NSLog(@"format: %s", (const char *)FourCC2Str(CVPixelBufferGetPixelFormatType(buffer)));
    
    if(buffer == NULL) {
        return;
    }
    
    if(self.firstTimestamp == INT64_MAX) {
        self.firstTimestamp = [self.timer getTime];
    }
    
    self.lastTimestamp = [self.timer getTime] - self.firstTimestamp;
    
    CMSampleBufferRef newSampleBuffer = NULL;
    CMSampleTimingInfo timingInfo = kCMTimingInfoInvalid;
    timingInfo.duration = CMTimeMake(33, 1000);    // assuming 30fps, change if otherwise
    timingInfo.decodeTimeStamp = CMTimeMake(self.lastTimestamp, 1000);    // timestamp information required
    timingInfo.presentationTimeStamp = timingInfo.decodeTimeStamp;
    
    CMVideoFormatDescriptionRef videoInfo = NULL;
    CMVideoFormatDescriptionCreateForImageBuffer(NULL, buffer, &videoInfo);
    
    CVPixelBufferLockBaseAddress(buffer, 0);
    CMSampleBufferCreateForImageBuffer(kCFAllocatorDefault,buffer,true,NULL,NULL,videoInfo,&timingInfo,&newSampleBuffer);
    
    // the following line submits the new CMSampleBufferRef to the nanostreamAVC lib
    [self.myVideoOutput.sampleBufferDelegate captureOutput:self.myVideoOutput didOutputSampleBuffer:newSampleBuffer fromConnection:nil];
    
    CVPixelBufferUnlockBaseAddress(buffer, 0);
    
    CFRelease(videoInfo);
    //CFRelease(buffer);
    CFRelease(newSampleBuffer);
    
}

@end
