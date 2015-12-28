
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

#import "ReginaHelper.h"
#import "TextViewController.h"
#import "packet/ntext.h"

@interface TextViewController () <UITextViewDelegate> {
    BOOL myEdit;
}
@property (weak, nonatomic) IBOutlet UITextView *detail;
@property (assign, nonatomic) regina::NText* packet;
@end

@implementation TextViewController

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

    self.detail.delegate = self;

    [self reloadPacket];

    NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
    [nc addObserver:self selector:@selector(keyboardDidShow:) name:UIKeyboardDidShowNotification object:nil];
    [nc addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];

    NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
    [nc removeObserver:self name:UIKeyboardDidShowNotification object:nil];
    [nc removeObserver:self name:UIKeyboardWillHideNotification object:nil];
}

- (void)reloadPacket {
    if (myEdit)
        return;

    self.detail.text = [NSString stringWithUTF8String:self.packet->getText().c_str()];
    [self.detail scrollRangeToVisible:NSMakeRange(0, 0)];
    // TODO: It still doesn't scroll quite far enough up (same for scripts also).
    // Find out what we need to set the contentOffset to.
}

- (void)endEditing
{
    // As a consequence, this calls textViewDidEndEditing:,
    // which is where the real work is done.
    [self.detail resignFirstResponder];
}

- (void)textViewDidEndEditing:(UITextView *)textView
{
    myEdit = YES;
    self.packet->setText([textView.text UTF8String]);
    myEdit = NO;
}

- (void)textViewDidChange:(UITextView *)textView
{
    // Grumph.  iOS 7 (but not iOS 8) lets the cursor move beneath
    // the keyboard as you type.  Ensure that the cursor remains visible.
    if (! [ReginaHelper ios8])
        [self ensureCursorVisible];
}

- (void)ensureCursorVisible
{
    // Sigh.  If we call [self.detail scrollRangeToVisible:self.detail.selectedRange],
    // the cursor does not move up above the keyboard (happens in both iOS 7 and iOS 8).
    // Scroll manually instead.
    CGRect caret = [self.detail caretRectForPosition:self.detail.selectedTextRange.end];
    CGFloat overflow = caret.origin.y + caret.size.height - (self.detail.contentOffset.y + self.detail.bounds.size.height - self.detail.contentInset.bottom /* keyboard height */);
    if (overflow > 0) {
        // Add a few points more than the required offset.
        [UIView animateWithDuration:.1 animations:^{
            self.detail.contentOffset = CGPointMake(self.detail.contentOffset.x,
                                                    self.detail.contentOffset.y + overflow + 7);
        }];
    }
}

- (void)keyboardDidShow:(NSNotification*)notification
{
    // Madly, the (width, height) for kbSize seem to switch positions between iOS 7 and iOS 8
    // when working in landscape mode.
    // What we want seems to be:
    // - kbSize.width for (iOS 7, landscape);
    // - kbSize.height for (iOS 7, portrait) and (iOS 8, all).
    // Just cheat and take the min.
    CGSize kbSize = [[[notification userInfo] objectForKey:UIKeyboardFrameBeginUserInfoKey] CGRectValue].size;
    CGFloat kbHeight = MIN(kbSize.width, kbSize.height);

    self.detail.contentInset = UIEdgeInsetsMake(0, 0, kbHeight, 0);
    self.detail.scrollIndicatorInsets = UIEdgeInsetsMake(0, 0, kbHeight, 0);

    [self ensureCursorVisible];
}

- (void)keyboardWillHide:(NSNotification*)notification
{
    self.detail.contentInset = UIEdgeInsetsZero;
    self.detail.scrollIndicatorInsets = UIEdgeInsetsZero;
}

@end
