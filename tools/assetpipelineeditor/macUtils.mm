#import <AppKit/AppKit.h>

extern "C" void disablemacOsTabs() {
    [NSWindow setAllowsAutomaticWindowTabbing: NO];
}
