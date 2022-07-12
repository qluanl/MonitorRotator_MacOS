//
//  main.c
//  MonitorRotator
//
//  Created by 李晟皋 on 2022/7/12.
//

#include <stdio.h>
#include <getopt.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>

#define MAX_DISPLAYS 16

enum {
    kIOFBSetTransform = 0x00000400,
};

IOOptionBits
angle2options(long angle)
{
    static IOOptionBits anglebits[] = {
        (kIOFBSetTransform | (kIOScaleRotate0)   << 16),
        (kIOFBSetTransform | (kIOScaleRotate90)  << 16),
        (kIOFBSetTransform | (kIOScaleRotate180) << 16),
        (kIOFBSetTransform | (kIOScaleRotate270) << 16)
    };
    
    if ((angle % 90) != 0) // Map arbitrary angles to a rotation reset
        return anglebits[0];
    
    return anglebits[(angle / 90) % 4];
}

int main(int argc, const char * argv[]) {
    // insert code here...
//    printf("Hello, World!\n");
//    return 0;
    
    CGDisplayErr      dErr;
    CGDisplayCount    displayCount, i;
    CGDirectDisplayID mainDisplay;
    CGDisplayCount    maxDisplays = MAX_DISPLAYS;
    CGDirectDisplayID onlineDisplays[MAX_DISPLAYS];
    
    mainDisplay = CGMainDisplayID();
    
    dErr = CGGetOnlineDisplayList(maxDisplays, onlineDisplays, &displayCount);
    if (dErr != kCGErrorSuccess) {
        fprintf(stderr, "CGGetOnlineDisplayList: error %d.\n", dErr);
        exit(1);
    }
    
    if (argc == 3 && atoi(argv[1])>0 ) {
        io_service_t      service;
        CGDirectDisplayID targetDisplay = onlineDisplays[atoi(argv[1])];
        IOOptionBits      options;
        
        long rotAng = atol(argv[2]);
        if ((rotAng % 90) != 0) {
            printf("Not feasible angle: %ld\n", rotAng);
        }else{
            printf("Rotating #%d to: %ld\n", atoi(argv[1]), rotAng);
            
            options = angle2options(rotAng);
            service = CGDisplayIOServicePort(targetDisplay);
            
            dErr = IOServiceRequestProbe(service, options);
            if (dErr != kCGErrorSuccess) {
                fprintf(stderr, "IOServiceRequestProbe: error %d\n", dErr);
                exit(1);
            }
        }
    }else{
        
        printf("Number  Display ID       Resolution\n");
        for (i = 0; i < displayCount; i++) {
            CGDirectDisplayID dID = onlineDisplays[i];
            printf("%d       0x%-14x %lux%lu %32s", i, dID,
                   CGDisplayPixelsWide(dID), CGDisplayPixelsHigh(dID),
                   (dID == mainDisplay) ? "[main display]\n" : "\n");
        }
        
    }
    
    
    
}


