#import <UIKit/UIKit.h>

@interface QIOSViewController
@end;

@interface QIOSViewController (MagicDisplay)
@end;

@implementation QIOSViewController (MagicDisplay)
- (UIStatusBarStyle)preferredStatusBarStyle
{
    return UIStatusBarStyleLightContent; // or UIStatusBarStyleDefault
}
@end
