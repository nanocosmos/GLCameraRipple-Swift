//
//  nanostreamAVCRtmpSourceCaptureSession.h
//  nanostreamAVC
//
//  Copyright (c) 2015-2017 nanocosmos. All rights reserved.
//  @version V. 4.8.9.2, January 2017

#ifndef nanostreamAVC_nanostreamAVCRtmpSourceCaptureSession_h
#define nanostreamAVC_nanostreamAVCRtmpSourceCaptureSession_h

#import "nanostreamAVC.h"
#import "nanostreamAVCExtendedCaptureSession.h"

/**
 * NSXRtmpSourceError
 * nanoStream RTMP Source Error types
 */
typedef NS_ENUM(NSInteger, NSXRtmpSourceError) {
    NSXRtmpSourceErrorNone,
    NSXRtmpSourceErrorLicenseInvalid,
    NSXRtmpSourceErrorLicenseFeatureMissing,
    NSXRtmpSourceErrorTCPConnectFailed,
    NSXRtmpSourceErrorSSLConnectFailed,
    NSXRtmpSourceErrorRTMPConnectFailed,
    NSXRtmpSourceErrorRTMPHandshakeFailed,
    NSXRtmpSourceErrorRTMPAuthFailed,
    NSXRtmpSourceErrorRTMPAppInvalid,
    NSXRtmpSourceErrorRTMPStreamNotFound,
    NSXRtmpSourceErrorSeekNotAvailable,
    NSXRtmpSourceErrorSeekPositionOutOfBounds,
    NSXRtmpSourceErrorSeekFailed
};

typedef NS_ENUM(NSInteger, NSXRtmpSourceStopReason) {
    NSXRtmpSourceStopReasonStreamComplete,
    NSXRtmpSourceStopReasonStreamNotAvailable,
    NSXRtmpSourceStopReasonStreamStopped,
    NSXRtmpSourceStopReasonNoMoreDataAvailable
};

@interface NSXRtmpSourceStatistics : NSObject

// the latency - time from sending to receiving
@property (nonatomic, assign) int latencyVideo; // latency for video in milliseconds
@property (nonatomic, assign) int latencyAudio; // latency for audio in milliseconds

// the complete latency - time from sending to rendering
@property (nonatomic, assign) int latencyVideoRendering;    // in milliseconds
@property (nonatomic, assign) int latencyAudioRendering;    // in milliseconds

// diff between last and first buffered packet in ms
@property (nonatomic, assign) int bufferDistanceVideoMs;
@property (nonatomic, assign) int bufferDistanceAudioMs;

@property (nonatomic, assign) int bufferedVideoPacketsRtmp;
@property (nonatomic, assign) int bufferedAudioPacketsRtmp;

@property (nonatomic, assign) int bufferedVideoPackets;
@property (nonatomic, assign) int bufferedAudioPackets;

// duration of one packet in ms
@property (nonatomic, assign) int packetDurationVideo;
@property (nonatomic, assign) int packetDurationAudio;

// key frame interval/distance in ms
@property (nonatomic, assign) int keyFrameInterval;

@end

/// RtmpSourceCaptureSessionStatusListener
@protocol RtmpSourceCaptureSessionStatusListener <NSObject>

/*
 * If the following method is not implemented on app level no video frames will be rendered.
 * Sample implication for managing the display layer on app level:
 *
- (void) didUpdateDisplayLayer: (CALayer*) layer {
    
    // UI View is modified, main queue required
    dispatch_async(dispatch_get_main_queue(), ^{
        if([self.playerView.layer sublayers] == nil) {
            [self.playerView.layer addSublayer: layer];
        }
        else {
            // replace display layer with new one
            for (CALayer* subLayer in [self.playerView.layer sublayers]) {
                if([subLayer isKindOfClass:AVSampleBufferDisplayLayer.class]) {
                    [self.playerView.layer replaceSublayer: subLayer with: layer];
                }
            }
        }
    });
}
 *
 */
- (void) didUpdateDisplayLayer: (CALayer*) layer;

/// RtmpSourceCaptureSessionStatusListener events
@optional
- (void) didStartPlayback;
- (void) didStartBuffering;
- (void) didStartBufferingDuringPlayback: (BOOL) underflow;
- (void) didResumePlayback;
- (void) didStopStreamWithReason: (NSXRtmpSourceStopReason) reason;
- (void) didFailToConnectWithError: (NSXRtmpSourceError) error withDescription: (NSString*) description;

- (void) statistics: (NSXRtmpSourceStatistics*) stats;

// can be used to fetch meta data from the sender (encoder)
// a sample usage would be to inform the player about a rotate of the sender device
// sample json string: {"onStatus":{"code":"NetStream.Data.Start"}}
- (void) didReceiveMetaData: (NSString*) metaDataAsJson;

@end

/*
 * interface RtmpSourceCaptureSession
 * to be used for RTMP Playback
 */
@interface RtmpSourceCaptureSession : ExtendedCaptureSession <ExtendedCaptureSessionStatusListener>

/// nanoStream license key
@property (nonatomic, copy) NSString* license;

// the delegate for events
@property (nonatomic, unsafe_unretained) id<RtmpSourceCaptureSessionStatusListener> delegate; // assign == weak for non-ARC

/** 
 RTMP Information:
 URL and Stream Name: rtmp://server/app/streamId
 */

/// URL: rtmp://server/app
@property (nonatomic, copy) NSString* url;

/// stream: rtmp://server/app/streamId
@property (nonatomic, copy) NSString* streamId;

/// RTMP Auth
@property (nonatomic, copy) NSString* user;
@property (nonatomic, copy) NSString* password;

/**
 * receiveTimeout in ms
 */
@property (nonatomic, assign) NSInteger receiveTimeout;

/**
 * minBufferDuration: player buffer in ms
 */
@property (nonatomic, assign) NSInteger minBufferDuration;

/**
 * maxAdditionalBufferDuration: Max. additional player buffer in ms.
 * If the buffer duration is higher than minBufferDuration + maxAdditionalBufferDuration,
 * a re-buffering is triggered, with the intention to reduce the latency.
 * default = 1000 (in ms), min. value = 500, 0 means no re-buffering will be triggered independent of the buffer duration.
 */
@property (nonatomic, assign) NSInteger maxAdditionalBufferDuration;

/**
 * endlessMode
 * default = NO (should usually be used)
 * YES: does not stop streaming when no data arrives
 */
@property (nonatomic, assign) BOOL endlessMode;

/**
 * Use acoustic echo cancellation
 *
 * In order for AEC to work the RTMPSourceCaptureSession should be run with nanostreamAVC in parallel.
 *
 * If the property usesApplicationAudioSession is set to YES, the category AVAudioSessionCategoryPlayAndRecord 
 * should be set for the audio session, otherwise AEC will not be supported.
 *
 * The default setting is NO.
 */
@property (assign) BOOL useAEC;

/**
 * Video playback requires iOS 8 or higher versions.
 * On iOS 7 and lower, only audio playback is supported.
 *
 * The method "supportedOnInstallediOSVersion" can be used to check, if video playback is available.
 */
@property (nonatomic, assign) BOOL videoPlayback;

@property (nonatomic, assign) BOOL audioPlayback;

/**
 * If you want manage the audio session by your own, this property has to be set to YES.
 *
 * Be aware that the correct audio session mode and category is required to use AEC (category: AVAudioSessionCategoryPlayAndRecord and mode: AVAudioSessionModeVideoChat).
 * If you set usesApplicationAudioSession to NO the RTMPSourceCaptureSession set the correct audio session category and mode.
 *
 * If the RTMPSourceCaptureSession should run in parallel with the encoder (nanostreamAVC) without AEC, this property has to be set to YES.
 *
 * The default setting is NO.
 */
@property(nonatomic) BOOL usesApplicationAudioSession;

- (id) init NS_UNAVAILABLE;

/**
 * init with bounds from your UIView
 */
- (id) initWithLayerBounds: (CGRect) bounds andLogLevel: (nanostreamLogLevel) logLevel;

/**
 *  Returns the display layer that is used for video playback
 *
 *  @return Display layer for the video.
 */
- (AVSampleBufferDisplayLayer *)displayLayer;

/**
 *  Set the video gravity.
 *  Options are AVLayerVideoGravityResizeAspect, AVLayerVideoGravityResizeAspectFill
 *  and AVLayerVideoGravityResize. AVLayerVideoGravityResizeAspect is default.
 */
- (void)setVideoGravityForPreview:(NSString *)videoGravityForPreview;

// use asynchronous calls start and stop instead
- (void)startRunning NS_UNAVAILABLE;
- (void)stopRunning NS_UNAVAILABLE;

#pragma mark - Playback Control

- (BOOL)canPause;
- (BOOL)canSeekBackward;
- (BOOL)canSeekForward;
- (int)currentPosition;
- (int)duration;
- (BOOL)isPlaying;
- (void)pause: (void (^)())completionBlock;
- (void)seekToPosition:(int)position withCompletion:(void (^)(NSXRtmpSourceError err))completion;
- (void)start: (void (^)())completionBlock;
- (void)stop: (void (^)())completionBlock;
- (int)state;

@end

#endif
