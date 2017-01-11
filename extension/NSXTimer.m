//
//  NSXTimer.m
//
//  Created by Oliver Arnold on 25/5/16.
//
//  @copyright (c) 2016 nanocosmos. All rights reserved.
//  http://www.nanocosmos.de

#import "NSXTimer.h"
#include <sys/time.h>

@interface NSXTimer ()
{
    volatile BOOL stopped;
}
- (void) timerThread;
@end

@implementation NSXTimer

- (id) init
{
    self = [super init];
    if(self)
    {
        startTime = 0LL;
        time = 0LL;
        prev_time = 0LL;
        
        timeQueue = dispatch_queue_create("my.time.queue", NULL);
        
        stopped = NO;
        [NSThread detachNewThreadSelector: @selector(timerThread) toTarget:self withObject:NULL];
    }
    
    return self;
}

- (void) dealloc
{
    [self close];
}

- (void) close
{
    stopped = YES;
}

- (void) reset
{
    dispatch_sync(timeQueue, ^{
        time = 0;
        prev_time = [self getTime];
    });
}

- (int64_t) getTime
{
    __block int64_t ret;
    dispatch_sync(timeQueue, ^{
        ret = time;
    });
    
    return ret;
}

- (void) timerThread
{
    while(stopped == NO)
    {
        [NSThread sleepForTimeInterval: .004];
        
        dispatch_sync(timeQueue, ^{
            struct timeval now;
            gettimeofday(&now, NULL);
            long curr_time = now.tv_sec * 1000L + now.tv_usec/1000L;
            
            if (curr_time >= prev_time)
            {
                time += curr_time - prev_time;
            }
            else {  // in case of overflow
                time += 4;
            }
            
            prev_time = curr_time;
        });
    }
}

@end
