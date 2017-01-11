//
//  nanostreamAVCHLSCaptureSession.h
//  nanostreamAVC
//
//  Copyright (c) 2015-2017 nanocosmos. All rights reserved.
//  @version V. 4.8.9.2, January 2017

#ifndef nanostreamAVCHLSCaptureSession_h
#define nanostreamAVCHLSCaptureSession_h

#import "nanostreamAVCExtendedCaptureSession.h"

/**
 * Can be used in combination with "initWithSession" to use a GoPro camera as input source for video and audio.
 * When the app goes to background [session stopRunning] can be used to stop the processing of the stream from the camera immediately.
 * If [session stopRunning] was invoked, [session startRunning] has to be invoked when the app comes back to the foreground.
 */

@interface HLSCaptureSession : ExtendedCaptureSession

- (id) init NS_UNAVAILABLE;
- (id) initWithLayerBounds: (CGRect) bounds andStatusListener:(id<ExtendedCaptureSessionStatusListener>)statusListener andUDP: (BOOL) udp;

@end

#endif /* nanostreamAVCHLSCaptureSession_h */
