/**
 * @file llpaneloutfitsinventory.cpp
 * @brief Outfits inventory panel
 *
 * $LicenseInfo:firstyear=2009&license=viewerlgpl$
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

#include "llpaneloutfitsinventory.h"

#include "llagentwearables.h"
#include "llappearancemgr.h"
#include "llfloatersidepanelcontainer.h"
#include "llinventoryfunctions.h"
#include "llinventorymodelbackgroundfetch.h"
#include "llnotificationsutil.h"
#include "lloutfitgallery.h"
#include "lloutfitobserver.h"
#include "lloutfitslist.h"
#include "llpanelappearancetab.h"
#include "llpanelwearing.h"
#include "llsidepanelappearance.h"
#include "lltabcontainer.h"
#include "llviewercontrol.h"
#include "llviewerfoldertype.h"

static const std::string OUTFITS_TAB_NAME = "outfitslist_tab";
static const std::string OUTFIT_GALLERY_TAB_NAME = "outfit_gallery_tab";
static const std::string COF_TAB_NAME = "cof_tab";

static const std::string SAVE_AS_BTN("save_as_btn");
static const std::string SAVE_BTN("save_btn");

static LLPanelInjector<LLPanelOutfitsInventory> t_inventory("panel_outfits_inventory");

LLPanelOutfitsInventory::LLPanelOutfitsInventory() :
    mMyOutfitsPanel(NULL),
    mCurrentOutfitPanel(NULL),
    mActivePanel(NULL),
    mAppearanceTabs(NULL),
    mInitialized(false)
{
    gAgentWearables.addLoadedCallback(boost::bind(&LLPanelOutfitsInventory::onWearablesLoaded, this));
    gAgentWearables.addLoadingStartedCallback(boost::bind(&LLPanelOutfitsInventory::onWearablesLoading, this));

    LLOutfitObserver& observer = LLOutfitObserver::instance();
    observer.addBOFChangedCallback(boost::bind(&LLPanelOutfitsInventory::updateVerbs, this));
    observer.addCOFChangedCallback(boost::bind(&LLPanelOutfitsInventory::updateVerbs, this));
    observer.addOutfitLockChangedCallback(boost::bind(&LLPanelOutfitsInventory::updateVerbs, this));

    // <FS:Ansariel> FIRE-17626: Attachment count in appearance floater
    mCategoriesObserver = new LLInventoryCategoriesObserver();
}

LLPanelOutfitsInventory::~LLPanelOutfitsInventory()
{
    if (mAppearanceTabs && mInitialized)
    {
        gSavedSettings.setS32("LastAppearanceTab", mAppearanceTabs->getCurrentPanelIndex());
    }

    // <FS:Ansariel> FIRE-17626: Attachment count in appearance floater
    if (gInventory.containsObserver(mCategoriesObserver))
    {
        gInventory.removeObserver(mCategoriesObserver);
    }
    delete mCategoriesObserver;
    // </FS:Ansariel>
}

// virtual
bool LLPanelOutfitsInventory::postBuild()
{
    initTabPanels();
    initListCommandsHandlers();

    // Fetch your outfits folder so that the links are in memory.
    // ( This is only necessary if we want to show a warning if a user deletes an item that has a
    // a link in an outfit, see "ConfirmItemDeleteHasLinks". )

    const LLUUID &outfits_cat = gInventory.findCategoryUUIDForType(LLFolderType::FT_MY_OUTFITS);
    if (outfits_cat.notNull())
    {
        LLInventoryModelBackgroundFetch::instance().start(outfits_cat);
    }

    getChild<LLButton>(SAVE_BTN)->setCommitCallback(boost::bind(&LLPanelOutfitsInventory::saveOutfit, this, false));
    getChild<LLButton>(SAVE_AS_BTN)->setCommitCallback(boost::bind(&LLPanelOutfitsInventory::saveOutfit, this, true));

    // <FS:Ansariel> FIRE-17626: Attachment count in appearance floater
    mTempAttachmentUpdateTimer.start();

    return true;
}

// virtual
void LLPanelOutfitsInventory::onOpen(const LLSD& key)
{
    if (!mInitialized)
    {
        LLSidepanelAppearance* panel_appearance = getAppearanceSP();
        if (panel_appearance)
        {
            // *TODO: move these methods to LLPanelOutfitsInventory?
            panel_appearance->fetchInventory();
            panel_appearance->refreshCurrentOutfitName();
        }

        // <FS:Ansariel> FIRE-17626: Attachment count in appearance floater
        gInventory.addObserver(mCategoriesObserver);
        mCategoriesObserver->addCategory(LLAppearanceMgr::instance().getCOF(), boost::bind(&LLPanelOutfitsInventory::onCOFChanged, this));
        onCOFChanged();
        // </FS:Ansariel>

        if (!mAppearanceTabs->selectTab(gSavedSettings.getS32("LastAppearanceTab")))
            mAppearanceTabs->selectFirstTab();

        mInitialized = true;
    }

    // Make sure we know which tab is selected, update the filter,
    // and update verbs.
    onTabChange();

    // *TODO: Auto open the first outfit newly created so new users can see sample outfit contents
    /*
    static bool should_open_outfit = true;
    if (should_open_outfit && gAgent.isFirstLogin())
    {
        LLInventoryPanel* outfits_panel = getChild<LLInventoryPanel>(OUTFITS_TAB_NAME);
        if (outfits_panel)
        {
            LLUUID my_outfits_id = gInventory.findCategoryUUIDForType(LLFolderType::FT_MY_OUTFITS);
            LLFolderViewFolder* my_outfits_folder = outfits_panel->getRootFolder()->getFolderByID(my_outfits_id);
            if (my_outfits_folder)
            {
                LLFolderViewFolder* first_outfit = dynamic_cast<LLFolderViewFolder*>(my_outfits_folder->getFirstChild());
                if (first_outfit)
                {
                    first_outfit->setOpen(true);
                }
            }
        }
    }
    should_open_outfit = false;
    */
}

void LLPanelOutfitsInventory::updateVerbs()
{
    if (mListCommands)
    {
        updateListCommands();
    }
}

// virtual
void LLPanelOutfitsInventory::onSearchEdit(const std::string& string)
{
    if (!mActivePanel) return;

    if (!LLInventoryModelBackgroundFetch::instance().inventoryFetchStarted())
    {
        llassert(false); // this should have been done on startup
        LLInventoryModelBackgroundFetch::instance().start();
    }

    // set new filter string
    mActivePanel->setFilterSubString(string);
}

void LLPanelOutfitsInventory::onWearButtonClick()
{
    if(isOutfitsListPanelActive())
    {
        if (mMyOutfitsPanel->hasItemSelected())
        {
            mMyOutfitsPanel->wearSelectedItems();
        }
        else
        {
            mMyOutfitsPanel->performAction("replaceoutfit");
        }
    }
    else if(isOutfitsGalleryPanelActive())
    {
        mOutfitGalleryPanel->wearSelectedOutfit();
    }

}

bool LLPanelOutfitsInventory::onSaveCommit(const LLSD& notification, const LLSD& response)
{
    S32 option = LLNotificationsUtil::getSelectedOption(notification, response);
    if (0 == option)
    {
        std::string outfit_name = response["message"].asString();
        LLStringUtil::trim(outfit_name);
        if( !outfit_name.empty() )
        {
            LLAppearanceMgr::getInstance()->makeNewOutfitLinks(outfit_name);

            LLSidepanelAppearance* panel_appearance = getAppearanceSP();
            if (panel_appearance)
            {
                panel_appearance->showOutfitsInventoryPanel();
            }

            if (mAppearanceTabs)
            {
                mAppearanceTabs->selectTabByName(OUTFITS_TAB_NAME);
            }
        }
    }

    return false;
}

void LLPanelOutfitsInventory::onSave()
{
    std::string outfit_name;

    if (!LLAppearanceMgr::getInstance()->getBaseOutfitName(outfit_name))
    {
        outfit_name = LLViewerFolderType::lookupNewCategoryName(LLFolderType::FT_OUTFIT);
    }

    LLSD args;
    args["DESC"] = outfit_name;

    LLSD payload;
    //payload["ids"].append(*it);

    LLNotificationsUtil::add("SaveOutfitAs", args, payload, boost::bind(&LLPanelOutfitsInventory::onSaveCommit, this, _1, _2));
}

// <FS:Ansariel> FIRE-17626: Attachment count in appearance floater
void LLPanelOutfitsInventory::draw()
{
    if (mTempAttachmentUpdateTimer.checkExpirationAndReset(1.f))
    {
        if (U32 tempAttachmentCount = (U32)LLAgentWearables::getTempAttachments().size(); tempAttachmentCount != mCurrentTempAttachmentCount)
        {
            mCurrentTempAttachmentCount = tempAttachmentCount;
            onCOFChanged();
        }
    }

    LLPanel::draw();
}

void LLPanelOutfitsInventory::onCOFChanged()
{
    if (!isAgentAvatarValid())
    {
        return;
    }

    const LLUUID cof = LLAppearanceMgr::instance().getCOF();
    LLInventoryModel::item_array_t obj_items;
    LLInventoryModel::cat_array_t cats;
    LLIsType is_of_type(LLAssetType::AT_OBJECT);
    gInventory.collectDescendentsIf(cof, cats, obj_items, LLInventoryModel::EXCLUDE_TRASH, is_of_type);
    U32 attachments = static_cast<U32>(obj_items.size()) + mCurrentTempAttachmentCount;

    LLStringUtil::format_map_t args;
    args["COUNT"] = llformat("%d", attachments);
    args["MAX"] = llformat("%d", gAgentAvatarp->getMaxAttachments());
    std::string title = getString("cof_tab_label", args);
    mAppearanceTabs->setPanelTitle(mAppearanceTabs->getIndexForPanel(mCurrentOutfitPanel), title);
}
// </FS:Ansariel>

//static
LLPanelOutfitsInventory* LLPanelOutfitsInventory::findInstance()
{
    return dynamic_cast<LLPanelOutfitsInventory*>(LLFloaterSidePanelContainer::getPanel("appearance", "panel_outfits_inventory"));
}

void LLPanelOutfitsInventory::openApearanceTab(const std::string& tab_name)
{
    if (!mAppearanceTabs) return;
    mAppearanceTabs->selectTabByName(tab_name);
}

//////////////////////////////////////////////////////////////////////////////////
// List Commands                                                                //

void LLPanelOutfitsInventory::initListCommandsHandlers()
{
    mListCommands = getChild<LLPanel>("bottom_panel");
    mWearBtn = mListCommands->getChild<LLButton>("wear_btn");
    mWearBtn->setCommitCallback(boost::bind(&LLPanelOutfitsInventory::onWearButtonClick, this));
    mMyOutfitsPanel->childSetAction("trash_btn", boost::bind(&LLPanelOutfitsInventory::onTrashButtonClick, this));
    mOutfitGalleryPanel->childSetAction("trash_btn", boost::bind(&LLPanelOutfitsInventory::onTrashButtonClick, this));
}

void LLPanelOutfitsInventory::updateListCommands()
{
    bool trash_enabled = isActionEnabled("delete");
    bool wear_enabled =  isActionEnabled("wear");
    bool wear_visible = !isCOFPanelActive();
    bool make_outfit_enabled = isActionEnabled("save_outfit");
    mMyOutfitsPanel->childSetEnabled("trash_btn", trash_enabled);
    mOutfitGalleryPanel->childSetEnabled("trash_btn", trash_enabled);
    mWearBtn->setEnabled(wear_enabled);
    mWearBtn->setVisible(wear_visible);
    getChild<LLButton>(SAVE_BTN)->setEnabled(make_outfit_enabled);
    mWearBtn->setToolTip(getString((!isOutfitsGalleryPanelActive() && mMyOutfitsPanel->hasItemSelected()) ? "wear_items_tooltip" : "wear_outfit_tooltip"));
}

void LLPanelOutfitsInventory::onTrashButtonClick()
{
    if(isOutfitsListPanelActive())
    {
        mMyOutfitsPanel->removeSelected();
    }
    else if(isOutfitsGalleryPanelActive())
    {
        mOutfitGalleryPanel->removeSelected();
    }
}

bool LLPanelOutfitsInventory::isActionEnabled(const LLSD& userdata)
{
    return mActivePanel && mActivePanel->isActionEnabled(userdata);
}

// List Commands                                                                //
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// Tab panels                                                                   //

void LLPanelOutfitsInventory::initTabPanels()
{
    //TODO: Add LLOutfitGallery change callback
    mCurrentOutfitPanel = findChild<LLPanelWearing>(COF_TAB_NAME);
    mCurrentOutfitPanel->setSelectionChangeCallback(boost::bind(&LLPanelOutfitsInventory::updateVerbs, this));

    mMyOutfitsPanel = findChild<LLOutfitsList>(OUTFITS_TAB_NAME);
    mMyOutfitsPanel->setSelectionChangeCallback(boost::bind(&LLPanelOutfitsInventory::updateVerbs, this));

    mOutfitGalleryPanel = findChild<LLOutfitGallery>(OUTFIT_GALLERY_TAB_NAME);
    mOutfitGalleryPanel->setSelectionChangeCallback(boost::bind(&LLPanelOutfitsInventory::updateVerbs, this));

    mAppearanceTabs = getChild<LLTabContainer>("appearance_tabs");
    mAppearanceTabs->setCommitCallback(boost::bind(&LLPanelOutfitsInventory::onTabChange, this));
}

void LLPanelOutfitsInventory::onTabChange()
{
    if (!mAppearanceTabs) return;
    mActivePanel = dynamic_cast<LLPanelAppearanceTab*>(mAppearanceTabs->getCurrentPanel());
    if (!mActivePanel) return;

    mActivePanel->checkFilterSubString();
    mActivePanel->onOpen(LLSD());

    updateVerbs();
}

bool LLPanelOutfitsInventory::isCOFPanelActive() const
{
    if (!mActivePanel) return false;

    return mActivePanel->getName() == COF_TAB_NAME;
}

bool LLPanelOutfitsInventory::isOutfitsListPanelActive() const
{
    if (!mActivePanel) return false;

    return mActivePanel->getName() == OUTFITS_TAB_NAME;
}

bool LLPanelOutfitsInventory::isOutfitsGalleryPanelActive() const
{
    if (!mActivePanel) return false;

    return mActivePanel->getName() == OUTFIT_GALLERY_TAB_NAME;
}

void LLPanelOutfitsInventory::setWearablesLoading(bool val)
{
    updateVerbs();
}

void LLPanelOutfitsInventory::onWearablesLoaded()
{
    setWearablesLoading(false);
}

void LLPanelOutfitsInventory::onWearablesLoading()
{
    setWearablesLoading(true);
}

// static
LLSidepanelAppearance* LLPanelOutfitsInventory::getAppearanceSP()
{
    LLSidepanelAppearance* panel_appearance =
        dynamic_cast<LLSidepanelAppearance*>(LLFloaterSidePanelContainer::getPanel("appearance"));
    return panel_appearance;
}

void LLPanelOutfitsInventory::saveOutfit(bool as_new)
{
    if (!as_new && LLAppearanceMgr::getInstance()->updateBaseOutfit())
    {
        // we don't need to ask for an outfit name, and updateBaseOutfit() successfully saved.
        // If updateBaseOutfit fails, ask for an outfit name anyways
        return;
    }

    onSave();
}

// <FS:Ansariel> Show avatar complexity in appearance floater
void LLPanelOutfitsInventory::updateAvatarComplexity(U32 complexity, const std::map<LLUUID, U32>& item_complexity, const std::map<LLUUID, U32>& temp_item_complexity, U32 body_parts_complexity)
{
    mOutfitGalleryPanel->updateAvatarComplexity(complexity);
    mMyOutfitsPanel->updateAvatarComplexity(complexity);
    mCurrentOutfitPanel->updateAvatarComplexity(complexity, item_complexity, temp_item_complexity, body_parts_complexity);
}
// </FS:Ansariel>
