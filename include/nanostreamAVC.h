/**
  @file nanostreamAVC.h
  nanostreamAVC
  AV Capture and stream publish from Live Cam with H.264/AAC

  @version V. 4.8.9.2, January 2017

  @copyright (c) 2015-2017 nanocosmos. All rights reserved.
  http://www.nanocosmos.de
  This is proprietary code and requires a valid usage license
*/

#define NANOSTREAM_AVC_VERSION 4892
#define NANOSTREAM_AVC_VERSION_STRING @"4.8.9.2"

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

// log level types
typedef NS_ENUM(NSInteger, nanostreamLogLevel) {
    LogLevelMinimal,    // default
    LogLevelModerate,
    LogLevelVerbose,
    LogLevelCustomSupport // only use if requested by support
};

/**
 * NSXError
 * nanoStream  Error types
 */
typedef NS_ENUM(NSInteger, NSXError) {
    NSXErrorNone = 0,
    NSXErrorUnexpected,             // unexpected/unknown error
    NSXErrorGeneral,                // general error
    NSXErrorLicenseInvalid,         // license invalid
    NSXErrorLicenseExpired,         // license expired
    NSXErrorNoVideoDevice,          // no video device available
    NSXErrorNoVideoInput,           // no video input (device or stream)
    NSXErrorNoAudioDevice,          // no audio device available
    NSXErrorNoAudioInput,           // no audio input (device or stream)
    NSXErrorInputFailed,            // general input error
    NSXErrorNoOutput,               // no output set
    NSXErrorOutputFailed,           // main output (primary) failed
    NSXErrorOutputFailed2,          // 2nd or other output failed
    NSXErrorNetworkFailed,          // general network failure
    NSXErrorServerConnectionFailed, // server connection failed
    NSXErrorAuthenticationFailed    // server auth failed
};

/// nanoStream Event types
typedef NS_ENUM(NSInteger, nanostreamEvent) {
    StreamStarted,
    StreamStopped,
    StreamErrorConnect,
    StreamError,
    StreamConnectionStatus,
    StreamSettingCropModeNotSupported,    // crop mode not supported for resolution
    GeneralError
};

/// Video Frame Crop Mode, e.g. 640x480 -> 640x360
typedef NS_ENUM(NSInteger, nanostreamCropMode) {
    NoCrop,
    CropToSquare,       // Square Video
    CropTo16By9,        // Crop to 16:9
    CropTo4By3,         // Crop to 4:3
    CropTo160x90,
    CropTo192x108,
    // crops landscape to 512x288, portrait to 288x512.
    // caution: selecting a nanoStreamResolution smaller than 640x480
    // is not supported with this following CropModes
    CropTo256x144,
    CropTo384x216,
    CropTo512x288,
    CropTo568x320,
    CropTo640x360,
    // caution: selecting a nanoStreamResolution smaller than 1280x720
    // is not supported with this following CropModes
    CropTo768x432,
    CropTo960x540
};

/// H264 Encoder Profile and Level
/// recommended default: MainAutoLevel, for low latency: BaselineAutoLevel
typedef NS_ENUM(NSInteger, nanoStreamH264ProfileLevel) {
    Baseline30,
    Baseline31,
    Baseline41,
    BaselineAutoLevel,
    Main30,
    Main31,
    Main32,
    Main41,
    MainAutoLevel,
    High40,
    High41,
    HighAutoLevel
};

/// Audio Channels (Mono/Stereo)
typedef NS_ENUM(NSInteger, nanoStreamAudioChannel) {
    Mono,
    Stereo
};

/// Stream Resolution (Camera)
typedef NS_ENUM(NSInteger, nanoStreamResolution) {
    Resolution192x144,
    Resolution352x288,
    Resolution480x360,
    Resolution640x480,
    Resolution1280x720,
    Resolution1920x1080
};

/// StreamType: Video/Audio
typedef NS_ENUM(NSInteger, NSXStreamType) {
    NSXStreamTypeAudioOnly,
    NSXStreamTypeAudioVideo,
    NSXStreamTypeVideoOnly
};

typedef NS_ENUM(NSInteger, NSXEncodingMode) {
    // dynamically chooses the best encoder for the system
    // currently always chooses NSXEncodingModeLegacy
    NSXEncodingModeAutomatic,
    // old method of encoding (on all devices, but required for <= iOS7)
    NSXEncodingModeLegacy,
    // only available from iOS8 and later
    NSXEncodingModeAdvancedInMemory
};

// The Rate Control Mode determines the strictness/tolerance that is used by the bitrate control.
// CBR Strict is the most strict setting. The tolerance is increasing from Constant Bitrate (CBR) to Average Bitrate (ABR) to Variable Bitrate (VBR)
typedef NS_ENUM(NSInteger, NSXH264RateControl) {
    NSXH264RateControlCBRStrict,
    NSXH264RateControlCBRStream,
    NSXH264RateControlCBRAuto,
    NSXH264RateControlABR1,
    NSXH264RateControlABR2,
    NSXH264RateControlVBR1,
    NSXH264RateControlVBR2
};

/**
 * # nanoStream Event handler
 */
@protocol nanostreamEventListener <NSObject>

/**
 * # Event Handler
 * possible results:
 * @param type        error/event type: StreamError, StreamErrorConnect, ...
 * @param description error/event description
 * @param level reserved for future use
 */
-(void) nanostreamEventHandlerWithType: (nanostreamEvent)type andLevel: (int)level andDescription: (NSString *)description;

@optional

-(void)nanoStreamStatisticsHandlerWithOutputBitrate:(long long)oBitrate andFillness:(double)fillness andCurrentVideoBitrate:(long long)vBitrate andCurrentVideoFramerate:(double)vFramerate;

/*!
 
 # willProcessFrame
 
 This delegate method will be called for every video sample that is processed.
 These are raw camera samples before any preprocessing or cropping.

 @discussion
 This method must not block for too long. Otherwise the stream and general 
 functionality may be impaired. For example, blocking 20ms per frame already 
 leads to general stream stability issues. Avoid using NSLog here as it is pretty slow. 

 You must not hold the CMSampleBufferRef for too long. If you need to keep them around
 consider making a copy of them and keep processing asynchronous.

 Modifying the CMSampleBufferRef is not supported.
 */
-(void)willProcessFrame:(CMSampleBufferRef)sample;

/**
 *  Calculates the average audio level for each packet of audio.
 *
 *  @param audioLevel Range is between -90dB and 0dB.
 *  Returns the current audio level or DBL_MAX if an error occurred.
 */
-(void)didMeasureAudioLevel:(double)audioLevel;

@end

// Adaptive Bitrate Mode
typedef NS_OPTIONS(NSInteger, AdaptiveBitrateControlMode) {
    AdaptiveBitrateControlDisabled              = 0,            // disabled ("constant" bit rate)
    AdaptiveBitrateControlModeQualityDegrade    = 1 << 0,       // change bitrate only (quality adjust)
    AdaptiveBitrateControlModeFrameDrop         = 1 << 1        // frame drop and bitrate adjust ("full")
};

// Adaptive Bitrate Settings
/*!     Settings for controlling the adaptive bitrate control (ABC).
 \member minimumBitrate the minimum bitrate the ABC should use as lowest bitrate setting, default is 50000 (~50 kb)
 \member maxPercentBitrateChange 10 (10%), 70=70%, etc., default: 50 (50%)
 \member minimumFramerate the minimum framerate the ABC should use as lowest framerate settings, default is 5 fps
 */
typedef struct {
    int minimumBitrate;             // values in kb: 50000-10000000(10Mb)
    int maxPercentBitrateChange;    // values in %: 0-100
    int minimumFramerate;           // values in frames per second: 5-60
} AdaptiveBitrateControlSettings;

// Local Recording Mode (Quality)
typedef NS_ENUM(NSInteger, nanoStreamLocalRecordingMode) {
    AVCRecordingModeOff,
    AVCRecordingModeStartBitrate,
    AVCRecordingModeDoubleAtLeastOneMbit,
    AVCRecordingMode720p2Mbit
};

typedef NS_ENUM(NSInteger, AVCOverlayBufferType) {
    AVCOverlayBufferTypeBGRA
};

/*
 Overlay
 */
typedef struct {
    UInt8 *buffer;
    int bytesPerRow;
    AVCOverlayBufferType bufferType;
} AVCOverlayRawBuffer;


/**
 *  Protocol that needs to be implemented by the overlay class
 */
@protocol AVCOverlay

/**
 *  Specifies the size that the AVCOverlayRawBuffer buffer needs to have.
 *  Will be set by the nanoStreamAVC object before calling
 *  overlayImageWithStreamTime: for the first time.
 */
@property (assign) CGSize imageSize;

/**
 *  Specifies the rect (of the image/buffer) which will get streamed after cropping.
 *  Only relevant if cropping is active, otherwise the full image will be used.
 *  Will be set by the nanoStreamAVC object before calling
 *  overlayImageWithStreamTime: for the first time.
 */
@property (assign) CGRect overlayBoundingRect;

/**
 *  Will be called by the nanoStreamAVC object for every frame of the stream.
 *  It is important that the method implementation is performant.
 */
-(AVCOverlayRawBuffer)overlayImageWithStreamTime:(NSTimeInterval)time;

@end


@interface AVCFullImageOverlay : NSObject <AVCOverlay>

- (id) initWithImage:(UIImage *)image;
- (void) dealloc;

@end

/**
 * Bandwidth Check
 * running an "empty" test stream to a specific rtmp url/stream
 * with a specific runTime after preroll time with maxBitrate
 */
@interface NSXBandwidthCheckSettings : NSObject

@property (copy) NSString* url;
@property (copy) NSString* streamId;
@property (assign) NSInteger preroll;
@property (assign) NSInteger runTime;
@property (assign) NSInteger maxBitrate;

@end

/**
 * Bandwitch Check Result
 * can be used to set the encoder bitrate
 */
@interface NSXBandwidthCheckResult : NSObject

@property (assign) NSInteger avgBitrate;
@property (assign) NSInteger medianBitrate;
@property (assign) NSInteger minBitrate;
@property (assign) NSInteger maxBitrate;
@property (assign) NSInteger runTimeMs;

@end

// Settings for a nanostreamAVC object
/*
 * Default settings are:
 * Video: 640x480@30fps, 512Kbit, Keyframe Distance = 2 seconds, h264 profile/level = MainAutoLevel, h264 Rate Control = NSXH264RateControlVBR2
 * Audio: 44.1kHz, stereo
 * Network: sendTimeout=2000ms, recvTimeout=1000ms
 */
@interface nanostreamAVCSettings : NSObject
// rtmp URL + stream id
@property (copy) NSString* url;                             // the RTMP URL: "rtmp://server/app"
@property (copy) NSString* streamId;                        // The stream ID: "myStream"

// camera settings
@property (assign) BOOL torch;                              // use torch
@property (assign) BOOL frontCamera;                        // use front or back camera
@property (assign) NSInteger focusMode;                     // use auto focus

// video settings
@property (assign) nanoStreamResolution videoResolution;    // video resolution, e.g. 640x480
@property (assign) NSInteger videoBitrate;                  // video bitrate 256, 512, ... [kbps]
@property (assign) float keyFrameInterval;                  // Key Frame Distance in seconds, 2-4 seconds recommended
@property (assign) AVCaptureVideoOrientation orientation;   // Orientation of camera and video as defined in AVCaptureSession.h
                                                            // AVCaptureVideoOrientationPortrait           = 1,
                                                            // AVCaptureVideoOrientationLandscapeRight     = 3
@property (assign) nanostreamCropMode cropMode;             // Determines if the video should be cropped (and how) or not
@property (assign) NSInteger framerate;
@property (assign) nanoStreamH264ProfileLevel h264Level;    // the h264 level used for encoding, default is MainAutoLevel
@property (assign) NSXH264RateControl h264RateControl;      // limits the maximum bitrate used by the h264 encoder, only available for NSXEncodingModeAdvancedInMemory

// audio settings
@property (assign) float initialVolume;                     // audioVolume: 0.0 to 1.0
@property (assign) nanoStreamAudioChannel audioMonoStereo;  // If the streamed and audio should be in mono or stereo (On Mac, this is always Stereo)
@property (assign) double audioSamplerate;                  // Audio Rate [/s], 44100.0f is recommended

@property (assign) NSInteger tcpSendTimeout;                // tcp send timeout in milliseconds,
@property (assign) NSInteger tcpRecvTimeout;                // tcp receive timeout in milliseconds

/**
 * # local recording
 *
 * nanoStreamLocalRecordingMode
 * default: AVCRecordingModeOff
 *
 */
@property (assign) nanoStreamLocalRecordingMode localRecordingMode;

/**
 * # localRecordingPath
 * contains path and file name, e.g. "/App/Documents/record.mp4",
 * the caller has to make sure that the library has permission to
 * write the file at the given location! default nil
 */
@property (copy) NSString* localRecordingPath;

/// streamType: default NSXStreamTypeAudioVideo
@property (assign) NSXStreamType streamType;

/// encodingMode: default NSXEncodingModeAutomatic
@property (assign) NSXEncodingMode encodingMode;

// This setting is important if you need custom audio settings for the AVAudioSession.
// When set to YES, the receiver ensures the application's audio session is set to the PlayAndRecord category, and picks an appropriate
// microphone and polar pattern to match the video camera being used. When set to NO, the receiver will use the application's audio session,
// but will not change any of its properties. Default value is YES
@property (assign) BOOL automaticallyConfiguresApplicationAudioSession;

- (id) init;    // init with default settings
- (void) dealloc;

@end

// nanoStreamAVC interface
@interface nanostreamAVC : NSObject

/*!     get nanostreamAVC version */
+(int) getVersion;

-(id)init NS_UNAVAILABLE;

/**     
 # initWithSettings
 
 Inits a new nanostreamAVC object
 
 Default way to init nanoStream
 
 @param settings an objects of nanostreamAVCSettings with the settings
 @param uiPreview the view for showing the video preview
 @param errorListener notification delegate for the streaming
 @returns new nanostreamAVC object
 */
-(id) initWithSettings: (nanostreamAVCSettings *) settings
             uiPreview: (UIView *) uiPreview                         // the view for showing the video preview
         errorListener: (id<nanostreamEventListener> ) errorListener; // set self class as the delegate


/**
 
 # initWithSession
 
 Inits a new nanostreamAVC object with an existing session
 
 @returns new nanostreamAVC object

 @description
 
 IMPORTANT: 
 it is expected that the following session properties:
    -init
    -setSessionPreset
    -addInput for audio and video (AVCaptureDeviceInput)
 are already set for the session! 
 The properties will not be set in this method - unlike when using the method initWithUrl.
 
 The methods startRunning and stopRunning of the AVCaptureSession also have to be invoked by the caller. 
 The only exception is the method start which will restart the session.
 
 When the nanostreamAVC object was created with initWithSession, 
 a call to useFrontCamera will exchange the first video input
 (if it exists) with either the front or the back camera.
 */
-(id) initWithSession: (AVCaptureSession*) session
             settings: (nanostreamAVCSettings *) settings
        errorListener: (id<nanostreamEventListener> ) errorListener; // set self class as the delegate

// close encoder
-(void) dealloc;

/**
 * Orientation of the stream
 */
@property (nonatomic, assign) AVCaptureVideoOrientation orientation;

/**
 *  Starts encoding/streaming, asynchronous
 *
 *  @param completionBlock success is true, when the connection to the server was established successfully, false otherwise
 */
-(void) start: (void (^)(NSXError error))completionBlock;

/**
 *  Stops streaming, asynchronous
 *
 *  @param sendAlreadyBufferedData if set, already buffered data (until the moment this stop method is called) will be sent to the server before stopping
 */
-(void) stop: (bool) sendAlreadyBufferedData withCompletion:(void (^)())completion;

/**
 *      Deprecated: Dont use this method anymore. Session stopped when nanostreamAVC is deallocated!
 */
-(void) stopSession __deprecated;

-(bool) active;

/**
 *  @brief
 *      make sure to set the license before invoking the method "start"
 */
-(int) setLicense: (const NSString*) license;

/**
 *  Sets the audio volume.
 *
 *  @param value the volume value, from 0.0 to 1.0
 */
-(void) setVolume: (float) value;

-(bool) setFocusMode: (AVCaptureFocusMode) mode;    // auto focus lock

/**
 *  Sets the Point of Interest for the Capture device and applies the given 
 *  focus mode afterwards.
 *
 *  @param mode The mode you want to set.
 *  @param poi  Point of interest. Values between (0,0) and (1,1). Specifies the POI in the capture devices coordinates. See [AVCaptureVideoPreviewLayer captureDevicePointOfInterestForPoint:].
 *
 *  @return YES if the mode and point were successfully set
 */
-(BOOL) setFocusMode:(AVCaptureFocusMode)mode withPoint:(CGPoint)poi;

-(AVCaptureFocusMode) getFocusMode;

-(void) useTorch: (bool) value; // not available in osx
-(bool) useFrontCamera: (bool) value;   // returns true if switch was successful

-(void) removeTemporaryFiles;

-(void) setAdaptiveBitrateControlMode:(AdaptiveBitrateControlMode)mode;
-(void) setAdaptiveBitrateControlSettings:(AdaptiveBitrateControlSettings)settings;

/// RTMP Auth
-(void) setAuthentication: (NSString*) user withPassword: (NSString*) password;

-(void) setWhiteBalanceMode: (AVCaptureWhiteBalanceMode) mode;

-(void) SetLogLevel: (nanostreamLogLevel) logLevel;

/**
 *  Changes the stream (rtmp) url.
 *  If a parameter is nil, the last setting will be used.
 *
 *  @param url      server url including the application name, e.g. "rtmp://localhost/live"
 *  @param streamID stream name, e.g. "myStream"
 */
-(void) setURL: (NSString *) url streamID: (NSString *) streamID;


/**
 *  Returns the size that will be used for the streamed video.
 *  Use this if you want to show the cropped region in the preview.
 *
 *  @return Size of video after cropping
 */
-(CGSize) videoSizeAfterCrop;

/**
 *  Returns the version of the library.
 *
 *  @return version as string
 */
+(NSString*) getVersionString;

/**
 *
 *  # sendMetadata
 *
 *  Sends Metadata to the streaming server. Metadata is passed in JSON format 
 *  and will be converted to an AMF-object. Values will be converted to their 
 *  respective types in AMF. Strings and keys must be written with double quotes, 
 *  numbers and bools without. Example JSON:
 *  @code
 *  {
 *      "name" : "myName",
 *      "type" : "myType",
 *      "parameters" : {
 *          "number1" : 123,
 *          "bool1"   : true
 *      }
 *  }
 *  @endcode
 *  @see http://www.nanocosmos.net/v4/documentation/live_video_encoder_-_rtmp_metadata for more information.
 *
 *  @param handlerName The handler to which you want to pass the data. E.g. @"onMetaData" or @"onCuePoint"
 *  Other handler names are discouraged and may break existing downstream workflows, e.g. transcoding / file conversion
 *  @param jsonValues  Serialized JSON object. Cannot contain an array. May not be infinitely nested.
 *  @param type        For future use. You should use @"" for now.
 *  @param live        Specifies if the metadata should be global or in-stream. Should be true (in-stream) for most cases.
 *
 *  @return Returns an error if the JSON was not UTF-8 encoded or if there was something wrong in the lib.
 */
-(NSError*) sendMetadata: (NSString*) handlerName withValues: (NSString*) jsonValues withType: (NSString*) type withLive: (bool) live;

/**
 *  setConnectInfo
 *
 *  Set a JSON string to be delivered to the server on the next connect.
 *  @see http://www.nanocosmos.net/v4/documentation/live_video_encoder_-_rtmp_connect_data for more information.
 *
 *  @param jsonConnectInfo Serialized JSON object. Top-level object must be an object.
 *  Cannot contain arrays or other objects. May only contain key-value-pairs. 
 *  Example JSON:
 *  @code
 *  {
 *      "key1" : "value1",
 *      "key2" : 13,
 *      "key3" : false
 *  }
 *  @endcode
 *
 *  @return Returns an error if the JSON was not UTF-8 encoded or otherwise could not be used.
 */

-(NSError*) setConnectInfo: (NSString*) jsonConnectInfo;

/**
 *  maxZoomFactor
 *
 *  Returns the maximum zoom factor that can be set on the current device.
 *  This includes the zoom via upscaling.
 *
 *  @return Maximum Zoom factor that can be set on the current device
 */
-(CGFloat)maxZoomFactor;

/**
 *  Newer devices can use an almost-optical zoom, using the bigger sensor sizes.
 *  Setting the zoom factor to a value smaller than this value uses a lossless
 *  zoom.
 *
 *  @return Max Zoom factor that can be set without using upscaling.
 */
-(CGFloat)maxZoomFactorWithoutUpscaling;


/**
 *  Sets the zoom factor for the camera. Available from iOS 7.1 onwards.
 *
 *  @param factor Double between 1.0 and maxZoomFactor. Will be set for the 
 *  current capture device. Will be reset when the device changes (camera rotation)
 *  Zoom factor will be applied with a ramp function, which results in a smooth
 *  transition to the given factor. If a value smaller than 
 *  maxZoomFactorWithoutUpscaling is set, the zoom will be lossless.
 *
 *  @return YES if zoom was set. NO if not.
 */
-(BOOL)setZoomFactor:(CGFloat)factor;


-(void)setOverlay: (NSObject<AVCOverlay>*) overlay;

/**
 *  Returns the preview layer that is used for the camera image. 
 *  After -stopSession was called, this will return nil.
 *
 *  @return Preview layer for the camera image or nil if initWithSession was used.
 */
-(AVCaptureVideoPreviewLayer *)previewLayer;

/**
 *  # runBandwidthCheck
 *
 *  Check the available bandwidth to the server before the real stream is started.
 */
-(void)runBandwidthCheck: (NSXBandwidthCheckSettings*) settings withCompletionBlock: (void (^)(NSXBandwidthCheckResult* measuredBandwidth))completionBlock;

-(void)stopBandwidthCheck;

/**
 * # grabStillImageWithCompletionBlock
 *
 * Grabs the next frame of the video and returns the UIImage image in the completion block
 */
-(void)grabStillImageWithCompletionBlock:(void (^)(UIImage *image, NSError *error))completion;

/** 
 * # grabStillImageAsBase64WithCompletionBlock
 *
 * Grabs the next frame of the video and returns the JPEG in Base64 string image in the completion block
 */
-(void)grabStillImageAsBase64WithCompletionBlock:(void (^)(NSString *jpegBase64, NSError *error))completion;

@end
