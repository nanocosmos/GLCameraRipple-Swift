//
//  nanostreamAVCExtendedCaptureSession.h
//  nanostreamAVC
//
//  Copyright (c) 2015-2017 nanocosmos. All rights reserved.
//  @version V. 4.8.9.2, January 2017

#ifndef nanostreamAVC_nanostreamAVCExtendedCaptureSession_h
#define nanostreamAVC_nanostreamAVCExtendedCaptureSession_h

#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

typedef NS_ENUM(NSInteger, NSXVideoSource) {
    NSXVideoSourceInternalCamera = 0,
    // generic input stream formats
    NSXVideoSourceRtmp = 1,
    NSXVideoSourceHls = 2,
    NSXVideoSourceUdp = 3,
    // specific external devices, gopro, drones
    NSXVideoSourceGoPro3 = 4,
    NSXVideoSourceGoPro4 = 5,
    NSXVideoSourceParrotBebop = 6,
    NSXVideoSourceParrotBebopSkycontroller = 7,
    NSXVideoSourceDJIP2VPlus = 8
};

typedef NS_ENUM(NSInteger, NSXRunStatusError) {
    NSXRunStatusLicenseError = 0,
    NSXRunStatusErrorNoData = 1,
    NSXRunStatusErrorDecodeData = 2,
    NSXRunStatusDataTimeout = 3,
    NSXRunStatusErrorDataSent = 4,
    NSXRunStatusErrorGimbalLock = 5,
    NSXRunStatusErrorGimbalMotor = 6
};

@protocol ExtendedCaptureSessionStatusListener <NSObject>

- (void) didUpdateDisplayLayer: (CALayer*) layer;

@optional
-(void) didDisconnectFromNetwork: (NSXVideoSource) source withError: (int) error;
-(void) appManagerDidRegister: (NSXVideoSource) source withError: (int) error withErrorDescription: (NSString*)desc;
-(void) runStatus: (NSXVideoSource) source withError: (NSXRunStatusError) error;
-(void) version: (NSXVideoSource) source withCameraFirmware: (NSString*) camFirmware withControllerFirmware: (NSString*) controllerFirmware;

@end

@interface ExtendedCaptureSession : AVCaptureSession

/**
 * The bounds for the layer. Should be typically set to UIView.bounds.
 * Needs to be set on init (initWithLayerBounds)
 */
@property(nonatomic) CGRect bounds;

-(id) init NS_UNAVAILABLE;

-(id) initWithLayerBounds: (CGRect) bounds;
/**
 *      Deprecated: Dont use this method anymore. invalidateDisplayLink is an empty call and does nothing
 */
- (void) invalidateDisplayLink __deprecated;

+(BOOL) checkStreamAvailable;
-(void) getResolution: (int*) width andHeight: (int*) height;
+(BOOL) supportedOnInstallediOSVersion;

-(void) startRunning;
-(void) stopRunning;

/**
 *  Tries to detect external video sources
 *
 *  @return NSXVideoSourceInternalCamera if no external device could be detected
 */
+ (NSXVideoSource)detectExternalVideoSource;

@end

#endif
