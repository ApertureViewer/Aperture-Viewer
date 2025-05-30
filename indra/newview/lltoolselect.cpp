/**
 * @file lltoolselect.cpp
 * @brief LLToolSelect class implementation
 *
 * $LicenseInfo:firstyear=2001&license=viewerlgpl$
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

#include "llviewerprecompiledheaders.h"

#include "lltoolselect.h"

#include "llagent.h"
#include "llagentcamera.h"
#include "llviewercontrol.h"
#include "lldrawable.h"
#include "llhudicon.h"
#include "llmanip.h"
#include "llmenugl.h"
#include "llselectmgr.h"
#include "llviewermediafocus.h"
#include "lltoolmgr.h"
#include "llfloaterscriptdebug.h"
#include "llviewercamera.h"
#include "llviewermenu.h"
#include "llviewerobject.h"
#include "llviewerobjectlist.h"
#include "llviewerregion.h"
#include "llviewerwindow.h"
#include "llvoavatarself.h"
#include "llworld.h"
// [RLVa:KB] - Checked: RLVa-2.0.0
#include "rlvactions.h"
#include "rlvmodifiers.h"
#include "llfloaterreg.h"
// [/RLVa:KB]

constexpr F32 SELECTION_ROTATION_TRESHOLD = 0.1f;
constexpr F32 SELECTION_SITTING_ROTATION_TRESHOLD = 3.2f; //radian

LLToolSelect::LLToolSelect( LLToolComposite* composite )
:   LLTool( std::string("Select"), composite ),
    mIgnoreGroup( false )
{
}

// True if you selected an object.
bool LLToolSelect::handleMouseDown(S32 x, S32 y, MASK mask)
{
    // do immediate pick query
    bool pick_rigged = false; //gSavedSettings.getBOOL("AnimatedObjectsAllowLeftClick");
    static LLCachedControl<bool> select_invisible_objects(gSavedSettings, "SelectInvisibleObjects");
    static LLCachedControl<bool> select_reflection_probes(gSavedSettings, "SelectReflectionProbes");

    mPick = gViewerWindow->pickImmediate(x, y, select_invisible_objects, pick_rigged, false, true, select_reflection_probes);

    // Pass mousedown to agent
    LLTool::handleMouseDown(x, y, mask);

    return mPick.getObject().notNull();
}

// static
LLObjectSelectionHandle LLToolSelect::handleObjectSelection(const LLPickInfo& pick, bool ignore_group, bool temp_select, bool select_root)
{
    LLViewerObject* object = pick.getObject();
    if (select_root)
    {
        object = object->getRootEdit();
    }

// [RLVa:KB] - Checked: RLVa-2.1.0
    if ( (object) && (RlvActions::isRlvEnabled()) )
    {
        if (!RlvActions::canEdit(object))
        {
            if (!temp_select)
                return LLSelectMgr::getInstance()->getSelection();
            else if (LLToolMgr::instance().inBuildMode())
                LLToolMgr::instance().leaveBuildMode();
        }

        if ( (RlvActions::hasBehaviour(RLV_BHVR_FARTOUCH)) && ( (!object->isAttachment()) || (!object->permYouOwner())) )
        {
            static RlvCachedBehaviourModifier<float> s_nFartouchDist(RLV_MODIFIER_FARTOUCHDIST);
            float nFartouchDistSq = s_nFartouchDist * s_nFartouchDist;

            // User is allowed to edit/select this object if it's within their current fartouch distance
            if (dist_vec_squared(gAgent.getPositionAgent(), object->getPositionRegion()) > nFartouchDistSq)
            {
                // The object is out of range but we'll still allow them a temporary select (e.g. context menu) if the surface point is within range
                if (dist_vec_squared(gAgent.getPositionAgent(), pick.mIntersection) > 1.5f * 1.5f)
                {
                    // Even the surface point is out of range so deny them the hit
                    if ( (LLFloaterReg::instanceVisible("build")) && (pick.mKeyMask != MASK_SHIFT) && (pick.mKeyMask != MASK_CONTROL) )
                        LLSelectMgr::getInstance()->deselectAll();
                    return LLSelectMgr::getInstance()->getSelection();
                }
                else if (LLToolMgr::instance().inBuildMode())
                {
                    // Allow the selection but keep it temporary by pulling them out of build mode when they click too far
                    LLToolMgr::instance().leaveBuildMode();
                }
            }
        }
    }
// [/RLVa:KB]

    bool select_owned = gSavedSettings.getBOOL("SelectOwnedOnly");
    bool select_movable = gSavedSettings.getBOOL("SelectMovableOnly");
    // <FS:Ansariel> FIRE-14593: Option to select only copyable objects
    bool select_copyable = gSavedSettings.getBOOL("FSSelectCopyableOnly");
    // <FS:Ansariel> FIRE-17696: Option to select only locked objects
    bool select_locked = gSavedSettings.getBOOL("FSSelectLockedOnly");

    // *NOTE: These settings must be cleaned up at bottom of function.
    if (temp_select || LLSelectMgr::getInstance()->mAllowSelectAvatar)
    {
        gSavedSettings.setBOOL("SelectOwnedOnly", false);
        gSavedSettings.setBOOL("SelectMovableOnly", false);
        // <FS:Ansariel> FIRE-14593: Option to select only copyable objects
        gSavedSettings.setBOOL("FSSelectCopyableOnly", false);
        // <FS:Ansariel> FIRE-17696: Option to select only locked objects
        gSavedSettings.setBOOL("FSSelectLockedOnly", false);
        LLSelectMgr::getInstance()->setForceSelection(true);
    }

    bool extend_select = (pick.mKeyMask == MASK_SHIFT) || (pick.mKeyMask == MASK_CONTROL);

    // If no object, check for icon, then just deselect
    if (!object)
    {
        LLHUDIcon* last_hit_hud_icon = pick.mHUDIcon;

        if (last_hit_hud_icon && last_hit_hud_icon->getSourceObject())
        {
            LLFloaterScriptDebug::show(last_hit_hud_icon->getSourceObject()->getID());
        }
        else if (!extend_select)
        {
            LLSelectMgr::getInstance()->deselectAll();
        }
    }
    else
    {
        bool already_selected = object->isSelected();

        if (already_selected &&
            object->getNumTEs() > 0 &&
            !LLSelectMgr::getInstance()->getSelection()->contains(object,SELECT_ALL_TES))
        {
            const LLTextureEntry* tep = object->getTE(pick.mObjectFace);
            if (tep && !tep->isSelected() && !LLViewerMediaFocus::getInstance()->getFocusedObjectID().isNull())
            {
                // we were interacting with media and clicked on non selected face, drop media focus
                LLViewerMediaFocus::getInstance()->clearFocus();
                // selection was removed and zoom preserved by clearFocus(), continue with regular selection
                already_selected = false;
                extend_select = true;
            }
        }

        if ( extend_select )
        {
            if ( already_selected )
            {
                if ( ignore_group )
                {
                    LLSelectMgr::getInstance()->deselectObjectOnly(object);
                }
                else
                {
                    LLSelectMgr::getInstance()->deselectObjectAndFamily(object, true, true);
                }
            }
            else
            {
                if ( ignore_group )
                {
                    LLSelectMgr::getInstance()->selectObjectOnly(object, SELECT_ALL_TES);
                }
                else
                {
                    LLSelectMgr::getInstance()->selectObjectAndFamily(object);
                }
            }
        }
        else
        {
            // Save the current zoom values because deselect resets them.
            F32 target_zoom;
            F32 current_zoom;
            LLSelectMgr::getInstance()->getAgentHUDZoom(target_zoom, current_zoom);

            // JC - Change behavior to make it easier to select children
            // of linked sets. 9/3/2002
            if( !already_selected || ignore_group)
            {
                // ...lose current selection in favor of just this object
                LLSelectMgr::getInstance()->deselectAll();
            }

            if ( ignore_group )
            {
                LLSelectMgr::getInstance()->selectObjectOnly(object, SELECT_ALL_TES, pick.mGLTFNodeIndex, pick.mGLTFPrimitiveIndex);
            }
            else
            {
                LLSelectMgr::getInstance()->selectObjectAndFamily(object);
            }

            // restore the zoom to the previously stored values.
            LLSelectMgr::getInstance()->setAgentHUDZoom(target_zoom, current_zoom);
        }

        if (!gAgentCamera.getFocusOnAvatar() &&                                     // if camera not glued to avatar
            LLVOAvatar::findAvatarFromAttachment(object) != gAgentAvatarp &&    // and it's not one of your attachments
            // <FS:Ansariel> FIRE-8039: Prevent avatar from turning to selected object
            //object != gAgentAvatarp)                                  // and it's not you
            object != gAgentAvatarp &&                                  // and it's not you
            gSavedSettings.getBOOL("FSTurnAvatarToSelectedObject"))
            // </FS:Ansariel>
        {
            // have avatar turn to face the selected object(s)
            LLVector3d selection_center = LLSelectMgr::getInstance()->getSelectionCenterGlobal();
            selection_center = selection_center - gAgent.getPositionGlobal();
            LLVector3 selection_dir;
            selection_dir.setVec(selection_center);
            selection_dir.mV[VZ] = 0.f;
            selection_dir.normVec();
            if (!object->isAvatar() && gAgent.getAtAxis() * selection_dir < 0.6f)
            {
                LLQuaternion target_rot;
                target_rot.shortestArc(LLVector3::x_axis, selection_dir);
                gAgent.startAutoPilotGlobal(gAgent.getPositionGlobal(),
                                            "",
                                            &target_rot,
                                            NULL,
                                            NULL,
                                            MAX_FAR_CLIP /*stop_distance, don't care since we are looking, not moving*/,
                                            gAgentAvatarp->isSitting() ? SELECTION_SITTING_ROTATION_TRESHOLD : SELECTION_ROTATION_TRESHOLD);
            }
        }

        if (temp_select)
        {
            if (!already_selected)
            {
                LLViewerObject* root_object = (LLViewerObject*)object->getRootEdit();
                LLObjectSelectionHandle selection = LLSelectMgr::getInstance()->getSelection();

                // this is just a temporary selection
                LLSelectNode* select_node = selection->findNode(root_object);
                if (select_node)
                {
                    select_node->setTransient(true);
                }

                LLViewerObject::const_child_list_t& child_list = root_object->getChildren();
                for (LLViewerObject::child_list_t::const_iterator iter = child_list.begin();
                     iter != child_list.end(); iter++)
                {
                    LLViewerObject* child = *iter;
                    select_node = selection->findNode(child);
                    if (select_node)
                    {
                        select_node->setTransient(true);
                    }
                }

            }
        } //if(temp_select)
    } //if(!object)

    // Cleanup temp select settings above.
    if (temp_select ||LLSelectMgr::getInstance()->mAllowSelectAvatar)
    {
        gSavedSettings.setBOOL("SelectOwnedOnly", select_owned);
        gSavedSettings.setBOOL("SelectMovableOnly", select_movable);
        // <FS:Ansariel> FIRE-14593: Option to select only copyable objects
        gSavedSettings.setBOOL("FSSelectCopyableOnly", select_copyable);
        // <FS:Ansariel> FIRE-17696: Option to select only locked objects
        gSavedSettings.setBOOL("FSSelectLockedOnly", select_locked);
        LLSelectMgr::getInstance()->setForceSelection(false);
    }

    return LLSelectMgr::getInstance()->getSelection();
}

bool LLToolSelect::handleMouseUp(S32 x, S32 y, MASK mask)
{
    mIgnoreGroup = gSavedSettings.getBOOL("EditLinkedParts");

    handleObjectSelection(mPick, mIgnoreGroup, false);

    return LLTool::handleMouseUp(x, y, mask);
}

void LLToolSelect::handleDeselect()
{
    if( hasMouseCapture() )
    {
        setMouseCapture( false );  // Calls onMouseCaptureLost() indirectly
    }
}


void LLToolSelect::stopEditing()
{
    if( hasMouseCapture() )
    {
        setMouseCapture( false );  // Calls onMouseCaptureLost() indirectly
    }
}

void LLToolSelect::onMouseCaptureLost()
{
    // Finish drag

    LLSelectMgr::getInstance()->enableSilhouette(true);

    // Clean up drag-specific variables
    mIgnoreGroup = false;
}




