
/**************************************************************************
 *                                                                        *
 *  Regina - A Normal Surface Theory Calculator                           *
 *  iOS User Interface                                                    *
 *                                                                        *
 *  Copyright (c) 1999-2013, Ben Burton                                   *
 *  For further details contact Ben Burton (bab@debian.org).              *
 *                                                                        *
 *  This program is free software; you can redistribute it and/or         *
 *  modify it under the terms of the GNU General Public License as        *
 *  published by the Free Software Foundation; either version 2 of the    *
 *  License, or (at your option) any later version.                       *
 *                                                                        *
 *  As an exception, when this program is distributed through (i) the     *
 *  App Store by Apple Inc.; (ii) the Mac App Store by Apple Inc.; or     *
 *  (iii) Google Play by Google Inc., then that store may impose any      *
 *  digital rights management, device limits and/or redistribution        *
 *  restrictions that are required by its terms of service.               *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful, but   *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *  General Public License for more details.                              *
 *                                                                        *
 *  You should have received a copy of the GNU General Public             *
 *  License along with this program; if not, write to the Free            *
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,       *
 *  MA 02110-1301, USA.                                                   *
 *                                                                        *
 **************************************************************************/

#import "AppDelegate.h"
#import "DetailViewController.h"
#import "MasterViewController.h"
#import "PacketTreeController.h"
#import "ReginaHelper.h"
#import "MBProgressHUD.h"
#import "UIView+Toast.h"
#import "packet/npacket.h"

static UISplitViewController* split;
static UINavigationController* masterNav;
static UINavigationController* detailNav;
static MasterViewController* master;
static DetailViewController* detail;
BOOL ios8;

@implementation ReginaHelper

+ (UIViewController *)root
{
    return split;
}

+ (MasterViewController *)master
{
    return master;
}

+ (DetailViewController *)detail
{
    return detail;
}

+ (ReginaDocument *)document
{
    return detail.doc;
}

+ (PacketTreeController *)tree
{
    UIViewController* tree = masterNav.topViewController;
    if ([tree.class isSubclassOfClass:[PacketTreeController class]])
        return static_cast<PacketTreeController*>(tree);
    else
        return nil;
}

+ (PacketTreeController *)treeRoot
{
    for (UIViewController* c in masterNav.viewControllers)
        if ([c isKindOfClass:[PacketTreeController class]])
            return static_cast<PacketTreeController*>(c);
    return nil;
}

+ (void)viewPacket:(regina::NPacket *)packet
{
    BOOL display = (packet->getPacketType() != regina::PACKET_CONTAINER);
    if (display)
        detail.packet = packet;
    
    PacketTreeController* tree = [ReginaHelper tree];
    // Note: the following calls are safe even if tree == nil.
    [tree navigateToPacket:packet->getTreeParent()];
    
    // Hmm.  Selection does not always work because the subtree might not yet
    // be present in the master navigation controller (the animated transition
    // is still taking place at this point).
    // Ah well, this is minor.
    if (display)
        [tree selectPacket:packet];
}

+ (void)viewChildren:(regina::NPacket *)packet
{
    PacketTreeController* tree = [ReginaHelper tree];
    [tree navigateToPacket:packet];

    regina::NPacket* child = packet->getFirstTreeChild();

    if (child && ! child->getNextTreeSibling()) {
        detail.packet = packet->getFirstTreeChild();
        [tree selectPacket:child];
    }
}

+ (void)notify:(NSString *)message detail:(NSString *)detail
{
    // Using TSMessage:
    /*
    [TSMessage showNotificationInViewController:split
                                          title:message
                                       subtitle:detail
                                           type:TSMessageNotificationTypeMessage];
    */

    // In portrait, display the banner in the top right corner so it is not obscured by
    // the master view controller.
    NSString* position = (UIInterfaceOrientationIsPortrait(split.interfaceOrientation) ? @"topright" : @"top");
    if (detail)
        [split.view makeToast:detail duration:3.0 position:position title:message];
    else
        [split.view makeToast:message duration:3.0 position:position];

}

+ (void)runWithHUD:(NSString *)message code:(void (^)())code cleanup:(void(^)())cleanup
{
    [UIApplication sharedApplication].idleTimerDisabled = YES;

    UIView* root = [UIApplication sharedApplication].keyWindow.rootViewController.view;
    MBProgressHUD* hud = [MBProgressHUD showHUDAddedTo:root animated:YES];
    hud.labelText = message;
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        code();
        dispatch_async(dispatch_get_main_queue(), ^{
            [MBProgressHUD hideHUDForView:root animated:NO];
            if (cleanup != nil)
                cleanup();

            [UIApplication sharedApplication].idleTimerDisabled = NO;
        });
    });
}

+ (BOOL)ios8
{
    return ios8;
}

+ (void)initWithApp:(AppDelegate *)app
{
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
        split = (UISplitViewController *)app.window.rootViewController;

        masterNav = [split.viewControllers firstObject];
        master = (id)masterNav.topViewController;

        detailNav = [split.viewControllers lastObject];
        detail = (id)detailNav.topViewController;

        split.delegate = detail;
    } else {
        split = nil;
        master = nil;
        detail = nil;
        masterNav = detailNav = nil;
    }

    ios8 = ([[[UIDevice currentDevice] systemVersion] compare:@"8.0" options:NSNumericSearch] != NSOrderedAscending);
    if (ios8)
        NSLog(@"Running on >= iOS 8");
    else
        NSLog(@"Running on iOS 7");
}

@end

