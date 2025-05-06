/**
 * @file llprogressbar.cpp
 * @brief LLProgressBar class implementation
 *
 * $LicenseInfo:firstyear=2002&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#include "linden_common.h"

#include "llprogressbar.h"

#include "indra_constants.h"
#include "llmath.h"
#include "llgl.h"
#include "llui.h"
#include "llfontgl.h"
#include "lltimer.h"
#include "llglheaders.h"

#include "llfocusmgr.h"
#include "lluictrlfactory.h"
#include "lluiimage.h"

static LLDefaultChildRegistry::Register<LLProgressBar> r("progress_bar");

LLProgressBar::Params::Params()
:   image_bar("image_bar"),
    image_fill("image_fill"),
    color_bar("color_bar"),
    color_bg("color_bg")
{}


LLProgressBar::LLProgressBar(const LLProgressBar::Params& p)
:   LLUICtrl(p),
    mImageBar(p.image_bar),
    mImageFill(p.image_fill),
    mColorBackground(p.color_bg()),
    mColorBar(p.color_bar()),
    mPercentDone(0.f)
{}

LLProgressBar::~LLProgressBar()
{
    gFocusMgr.releaseFocusIfNeeded( this );
}

void LLProgressBar::draw()
{
    // <AP:WW> Note: The static timer is still declared but won't be used for the pulsing effect anymore. </AP:WW>
    // <AP:WW> If it's not used elsewhere, it could potentially be removed, but leaving it is harmless. </AP:WW>
    static LLTimer timer;
    F32 alpha = getDrawContext().mAlpha; // Get the base alpha from the drawing context

    // --- Draw Background ---
    if (mImageBar) // optional according to parameters
    {
        LLColor4 image_bar_color = mColorBackground.get();
        image_bar_color.setAlpha(alpha); // Apply base alpha to background
        mImageBar->draw(getLocalRect(), image_bar_color);
    }

    // --- Draw Filled Portion ---
    if (mImageFill)
    {
        // <AP:WW> Disable the pulsing alpha effect by commenting out the calculation </AP:WW>
        // Calculate pulsing alpha effect - ORIGINAL LINE:
        // alpha *= 0.5f + 0.5f*0.5f*(1.f + (F32)sin(3.f*timer.getElapsedTimeF32()));
        // <AP:WW> By commenting out the line above, 'alpha' retains its original value </AP:WW>
        // <AP:WW> obtained from getDrawContext().mAlpha. </AP:WW>

        LLColor4 bar_color = mColorBar.get(); // Get the fill tint color
        // <AP:WW> Now apply the *unmodified* base alpha to the fill color's alpha </AP:WW>
        bar_color.mV[VALPHA] *= alpha; // modulate alpha using only the base alpha

        LLRect progress_rect = getLocalRect(); // Start with the full bar rectangle
        // Calculate the width of the filled portion based on percentage
        progress_rect.mRight = ll_round(getRect().getWidth() * (mPercentDone / 100.f));

        // Draw the fill image, clipped/scaled to progress_rect, using the bar_color (with non-pulsing alpha)
        mImageFill->draw(progress_rect, bar_color);
    }
}

void LLProgressBar::setValue(const LLSD& value)
{
    mPercentDone = llclamp((F32)value.asReal(), 0.f, 100.f);
}
