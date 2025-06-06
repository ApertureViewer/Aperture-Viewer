/**
 * @file llfloaterregioninfo.h
 * @author Aaron Brashears
 * @brief Declaration of the region info and controls floater and panels.
 *
 * $LicenseInfo:firstyear=2004&license=viewerlgpl$
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

#ifndef LL_LLFLOATERREGIONINFO_H
#define LL_LLFLOATERREGIONINFO_H

#include <vector>
#include "llagent.h"
#include "llassettype.h"
#include "llfloater.h"
#include "llhost.h"
#include "llpanel.h"
#include "llextendedstatus.h"
#include "llpanelenvironment.h"
#include "llvlcomposition.h"

#include "lleventcoro.h"

class LLAvatarName;
class LLDispatcher;
class LLLineEditor;
class LLMessageSystem;
class LLPanelRegionInfo;
class LLTabContainer;
class LLViewerRegion;
class LLViewerTextEditor;
class LLInventoryItem;
class LLCheckBoxCtrl;
class LLComboBox;
class LLNameListCtrl;
class LLRadioGroup;
class LLSliderCtrl;
class LLSpinCtrl;
class LLTextBox;
class LLTextureCtrl;

class LLPanelRegionGeneralInfo;
// <FS:CR> Aurora Sim - Region Settings Panel
class LLPanelRegionOpenSettingsInfo;
// </FS:CR> Aurora Sim - Region Settings Panel
class LLPanelRegionDebugInfo;
class LLPanelRegionTerrainInfo;
class LLPanelEstateInfo;
class LLPanelEstateCovenant;
class LLPanelExperienceListEditor;
class LLPanelExperiences;
class LLPanelRegionExperiences;
class LLPanelEstateAccess;
class LLPanelRegionEnvironment;

class LLEventTimer;

class LLFloaterRegionInfo : public LLFloater
{
    friend class LLFloaterReg;
public:


    void onOpen(const LLSD& key) override;
    void onClose(bool app_quitting) override;
    bool postBuild() override;

    static void processEstateOwnerRequest(LLMessageSystem* msg, void**);

    // get and process region info if necessary.
    static void processRegionInfo(LLMessageSystem* msg);
    static void sRefreshFromRegion(LLViewerRegion* region);

    static const LLUUID& getLastInvoice() { return sRequestInvoice; }
    static void nextInvoice() { sRequestInvoice.generate(); }
    //static S32 getSerial() { return sRequestSerial; }
    //static void incrementSerial() { sRequestSerial++; }

    static LLPanelEstateInfo* getPanelEstate();
    static LLPanelEstateAccess* getPanelAccess();
    static LLPanelEstateCovenant* getPanelCovenant();
    static LLPanelRegionTerrainInfo* getPanelRegionTerrain();
    static LLPanelRegionExperiences* getPanelExperiences();
    static LLPanelRegionGeneralInfo* getPanelGeneral();
    static LLPanelRegionEnvironment* getPanelEnvironment();
// <FS:CR> Aurora Sim - Region Settings Panel
    static LLPanelRegionOpenSettingsInfo* getPanelOpenSettings();
// </FS:CR> Aurora Sim - Region Settings Panel

    // from LLPanel
    void refresh() override;

    void onRegionChanged();
    void requestRegionInfo();
    void enableTopButtons();
    void disableTopButtons();

private:

    LLFloaterRegionInfo(const LLSD& seed);
    ~LLFloaterRegionInfo();

protected:
    void onTabSelected(const LLSD& param);
    void disableTabCtrls();
    void refreshFromRegion(LLViewerRegion* region);
    void onGodLevelChange(U8 god_level);

    // member data
    LLTabContainer* mTab;
    typedef std::vector<LLPanelRegionInfo*> info_panels_t;
    info_panels_t mInfoPanels;
    LLPanelRegionEnvironment *mEnvironmentPanel;
    //static S32 sRequestSerial;    // serial # of last EstateOwnerRequest
    static LLUUID sRequestInvoice;

private:
    LLAgent::god_level_change_slot_t   mGodLevelChangeSlot;
    boost::signals2::connection mRegionChangedCallback;
};


// Base class for all region information panels.
class LLPanelRegionInfo : public LLPanel
{
public:
    LLPanelRegionInfo();

    void onBtnSet();
    void onChangeChildCtrl(LLUICtrl* ctrl);
    void onChangeAnything();
    static void onChangeText(LLLineEditor* caller, void* user_data);

    virtual bool refreshFromRegion(LLViewerRegion* region);
    virtual bool estateUpdate(LLMessageSystem* msg) { return true; }

    bool postBuild() override;
    virtual void updateChild(LLUICtrl* child_ctrl);

    void enableButton(const std::string& btn_name, bool enable = true);
    void disableButton(const std::string& btn_name);

    void onClickManageTelehub();
    void onClickManageRestartSchedule();

protected:
    void initCtrl(const std::string& name);
    template<typename CTRL> void initAndSetCtrl(CTRL*& ctrl, const std::string& name);
    void initAndSetTexCtrl(LLTextureCtrl*& ctrl, const std::string& name);

    // Returns true if update sent and apply button should be
    // disabled.
    virtual bool sendUpdate() { return true; }

    typedef std::vector<std::string> strings_t;
    //typedef std::vector<U32> integers_t;
    void sendEstateOwnerMessage(
                     LLMessageSystem* msg,
                     const std::string& request,
                     const LLUUID& invoice,
                     const strings_t& strings);


    // member data
    LLHost mHost;
    LLHandle<LLFloater> mFloaterRestartScheduleHandle;
};

/////////////////////////////////////////////////////////////////////////////
// Actual panels start here
/////////////////////////////////////////////////////////////////////////////

// <FS:CR> Aurora Sim - Region Settings Panel
class LLPanelRegionOpenSettingsInfo : public LLPanelRegionInfo
{
public:
    LLPanelRegionOpenSettingsInfo()
        :   LLPanelRegionInfo() {}
    ~LLPanelRegionOpenSettingsInfo() {}

    virtual bool refreshFromRegion(LLViewerRegion* region);

    // LLPanel
    virtual bool postBuild();

protected:
    static void onClickOrs(void* userdata);
    static void onClickHelp(void* data);
};
/////////////////////////////////////////////////////////////////////////////
// </FS:CR> Aurora Sim - Region Settings Panel

class LLPanelRegionGeneralInfo : public LLPanelRegionInfo
{

public:
    LLPanelRegionGeneralInfo()
        :   LLPanelRegionInfo() {}
    ~LLPanelRegionGeneralInfo() {}

    bool refreshFromRegion(LLViewerRegion* region) override;

    bool postBuild() override;

    void onBtnSet();
    void setObjBonusFactor(F32 object_bonus_factor) {mObjBonusFactor = object_bonus_factor;}

protected:
    bool sendUpdate() override;
    void onClickKick();
    void onKickCommit(const uuid_vec_t& ids);
    static void onClickKickAll(void* userdata);
    bool onKickAllCommit(const LLSD& notification, const LLSD& response);
    static void onClickMessage(void* userdata);
    bool onMessageCommit(const LLSD& notification, const LLSD& response);
    bool onChangeObjectBonus(const LLSD& notification, const LLSD& response);

    F32 mObjBonusFactor;

};

/////////////////////////////////////////////////////////////////////////////

class LLPanelRegionDebugInfo : public LLPanelRegionInfo
{
public:
    LLPanelRegionDebugInfo()
        :   LLPanelRegionInfo(), mTargetAvatar() {}
    ~LLPanelRegionDebugInfo() {}

    bool postBuild() override;

    bool refreshFromRegion(LLViewerRegion* region) override;

protected:
    bool sendUpdate() override;

    void onClickChooseAvatar();
    void callbackAvatarID(const uuid_vec_t& ids, const std::vector<LLAvatarName> names);
    static void onClickReturn(void *);
    bool callbackReturn(const LLSD& notification, const LLSD& response);
    static void onClickTopColliders(void*);
    static void onClickTopScripts(void*);
    static void onClickRestart(void* data);
    bool callbackRestart(const LLSD& notification, const LLSD& response, const LLSD& seconds); // Ansariel, added seconds for FIRE-1073
    static void onClickCancelRestart(void* data);
    static void onClickDebugConsole(void* data);

private:
    LLUUID mTargetAvatar;
};

/////////////////////////////////////////////////////////////////////////////

class LLPanelRegionTerrainInfo : public LLPanelRegionInfo
{
    LOG_CLASS(LLPanelRegionTerrainInfo);

public:
    LLPanelRegionTerrainInfo();
    ~LLPanelRegionTerrainInfo() {}

    bool postBuild() override;

    bool refreshFromRegion(LLViewerRegion* region) override;                // refresh local settings from region update from simulator
    void setEnvControls(bool available);                                    // Whether environment settings are available for this region

    bool validateTextureSizes();
    bool validateMaterials();
    bool validateTextureHeights();

    //static void onChangeAnything(LLUICtrl* ctrl, void* userData);         // callback for any change, to enable commit button

    void onSelectMaterialType();
    void updateForMaterialType();

    static void onClickDownloadRaw(void*);
    static void onClickUploadRaw(void*);
    // <FS:Ansariel> Threaded filepickers
    void onDownloadRawFilepickerCB(const std::vector<std::string>& filenames);
    void onUploadRawFilepickerCB(const std::vector<std::string>& filenames);
    // </FS:Ansariel>
    static void onClickBakeTerrain(void*);
    bool callbackBakeTerrain(const LLSD& notification, const LLSD& response);
    bool callbackTextureHeights(const LLSD& notification, const LLSD& response);

protected:
    bool sendUpdate() override;

    void initMaterialCtrl(LLTextureCtrl*& ctrl, const std::string& name, S32 index);

private:
    bool mConfirmedTextureHeights;
    bool mAskedTextureHeights;
    LLCheckBoxCtrl* mMaterialTypeCtrl = nullptr;
    LLTextureCtrl* mTextureDetailCtrl[LLTerrainMaterials::ASSET_COUNT];
    LLTextureCtrl* mMaterialDetailCtrl[LLTerrainMaterials::ASSET_COUNT];

    LLUUID mLastSetTextures[LLTerrainMaterials::ASSET_COUNT];
    LLUUID mLastSetMaterials[LLTerrainMaterials::ASSET_COUNT];

    LLSpinCtrl* mMaterialScaleUCtrl[LLTerrainMaterials::ASSET_COUNT];
    LLSpinCtrl* mMaterialScaleVCtrl[LLTerrainMaterials::ASSET_COUNT];
    LLSpinCtrl* mMaterialRotationCtrl[LLTerrainMaterials::ASSET_COUNT];
    LLSpinCtrl* mMaterialOffsetUCtrl[LLTerrainMaterials::ASSET_COUNT];
    LLSpinCtrl* mMaterialOffsetVCtrl[LLTerrainMaterials::ASSET_COUNT];
};

/////////////////////////////////////////////////////////////////////////////

class LLPanelEstateInfo : public LLPanelRegionInfo
{
public:
    static void initDispatch(LLDispatcher& dispatch);

    void onChangeFixedSun();
    void onChangeUseGlobalTime();
    void onChangeAccessOverride();

    void onClickEditSky();
    void onClickEditSkyHelp();
    void onClickEditDayCycle();
    void onClickEditDayCycleHelp();

    void onClickKickUser();


    bool kickUserConfirm(const LLSD& notification, const LLSD& response);

    void onKickUserCommit(const uuid_vec_t& ids);
    static void onClickMessageEstate(void* data);
    bool onMessageCommit(const LLSD& notification, const LLSD& response);

    LLPanelEstateInfo();
    ~LLPanelEstateInfo();

    void updateControls(LLViewerRegion* region);

    static void updateEstateName(const std::string& name);
    static void updateEstateOwnerName(const std::string& name);

    bool refreshFromRegion(LLViewerRegion* region) override;
    bool estateUpdate(LLMessageSystem* msg) override;

    bool postBuild() override;
    void updateChild(LLUICtrl* child_ctrl) override;
    void refresh() override;

    void refreshFromEstate();

    static bool isLindenEstate();

    const std::string getOwnerName() const;
    void setOwnerName(const std::string& name);

protected:
    bool sendUpdate() override;
    // confirmation dialog callback
    bool callbackChangeLindenEstate(const LLSD& notification, const LLSD& response);

    void commitEstateAccess();
    void commitEstateManagers();

    bool checkSunHourSlider(LLUICtrl* child_ctrl);

    U32 mEstateID;
    boost::signals2::connection mEstateInfoCommitConnection;
    boost::signals2::connection mEstateInfoUpdateConnection;
};

/////////////////////////////////////////////////////////////////////////////

class LLPanelEstateCovenant : public LLPanelRegionInfo
{
public:
    LLPanelEstateCovenant();
    ~LLPanelEstateCovenant() {}

    bool postBuild() override;
    void updateChild(LLUICtrl* child_ctrl) override;
    bool refreshFromRegion(LLViewerRegion* region) override;
    bool estateUpdate(LLMessageSystem* msg) override;

    // LLView overrides
    bool handleDragAndDrop(S32 x, S32 y, MASK mask,
                           bool drop, EDragAndDropType cargo_type,
                           void *cargo_data, EAcceptance *accept,
                           std::string& tooltip_msg) override;
    static bool confirmChangeCovenantCallback(const LLSD& notification, const LLSD& response);
    static void resetCovenantID(void* userdata);
    static bool confirmResetCovenantCallback(const LLSD& notification, const LLSD& response);
    void sendChangeCovenantID(const LLUUID &asset_id);
    void loadInvItem(LLInventoryItem *itemp);
    static void onLoadComplete(const LLUUID& asset_uuid,
                               LLAssetType::EType type,
                               void* user_data, S32 status, LLExtStat ext_status);

    // Accessor functions
    static void updateCovenant(const LLTextBase* source, const LLUUID& asset_id);
    static void updateCovenantText(const std::string& string, const LLUUID& asset_id);
    static void updateEstateName(const std::string& name);
    static void updateLastModified(const std::string& text);
    static void updateEstateOwnerName(const std::string& name);

    const LLUUID& getCovenantID() const { return mCovenantID; }
    void setCovenantID(const LLUUID& id) { mCovenantID = id; }
    std::string getEstateName() const;
    void setEstateName(const std::string& name);
    std::string getOwnerName() const;
    void setOwnerName(const std::string& name);
    void setCovenantTextEditor(const std::string& text);

    typedef enum e_asset_status
    {
        ASSET_ERROR,
        ASSET_UNLOADED,
        ASSET_LOADING,
        ASSET_LOADED
    } EAssetStatus;

protected:
    bool sendUpdate() override;
    LLTextBox*              mEstateNameText;
    LLTextBox*              mEstateOwnerText;
    LLTextBox*              mLastModifiedText;
    // CovenantID from sim
    LLUUID                  mCovenantID;
    LLViewerTextEditor*     mEditor;
    EAssetStatus            mAssetStatus;
};

/////////////////////////////////////////////////////////////////////////////


class LLPanelRegionExperiences : public LLPanelRegionInfo
{
    LOG_CLASS(LLPanelRegionExperiences);

public:
    LLPanelRegionExperiences(){}
    bool postBuild() override;

    static bool experienceCoreConfirm(const LLSD& notification, const LLSD& response);
    static void sendEstateExperienceDelta(U32 flags, const LLUUID& agent_id);

    static void infoCallback(LLHandle<LLPanelRegionExperiences> handle, const LLSD& content);
    bool refreshFromRegion(LLViewerRegion* region) override;
    void sendPurchaseRequest()const;
    void processResponse( const LLSD& content );

protected:
    bool sendUpdate() override;

private:
    void refreshRegionExperiences();

    static std::string regionCapabilityQuery(LLViewerRegion* region, const std::string &cap);

    LLPanelExperienceListEditor* setupList(const char* control_name, U32 add_id, U32 remove_id);
    static LLSD addIds( LLPanelExperienceListEditor* panel );

    void itemChanged(U32 event_type, const LLUUID& id);

    LLPanelExperienceListEditor* mTrusted;
    LLPanelExperienceListEditor* mAllowed;
    LLPanelExperienceListEditor* mBlocked;
    LLUUID mDefaultExperience;
};


class LLPanelEstateAccess : public LLPanelRegionInfo
{
    LOG_CLASS(LLPanelEstateAccess);

public:
    LLPanelEstateAccess();

    bool postBuild() override;
    void updateChild(LLUICtrl* child_ctrl) override;

    void updateControls(LLViewerRegion* region);
    void updateLists();

    void setPendingUpdate(bool pending) { mPendingUpdate = pending; }
    bool getPendingUpdate() { return mPendingUpdate; }

    bool refreshFromRegion(LLViewerRegion* region) override;

    // <FS:Ansariel> Moved to public
    static void sendEstateAccessDelta(U32 flags, const LLUUID& agent_id);

    // <FS:PP> Ban and access lists export/import
    void onClickExportEstateManagerList();
    void onClickExportAllowedList();
    void onClickExportAllowedGroupList();
    void onClickExportBannedList();
    void onClickExportList(LLNameListCtrl* list, const std::string& filename);
    void exportListCallback(LLNameListCtrl* list, const std::vector<std::string>& filenames);
    void onClickImportEstateManagerList();
    void onClickImportAllowedList();
    void onClickImportAllowedGroupList();
    void onClickImportBannedList();
    void onClickImportList(LLNameListCtrl* list);
    void importListCallback(LLNameListCtrl* list, const std::vector<std::string>& filenames);
    // </FS:PP> Ban and access lists export/import

private:
    void onClickAddAllowedAgent();
    void onClickRemoveAllowedAgent();
    void onClickCopyAllowedList();
    void onClickAddAllowedGroup();
    void onClickRemoveAllowedGroup();
    void onClickCopyAllowedGroupList();
    void onClickAddBannedAgent();
    void onClickRemoveBannedAgent();
    void onClickCopyBannedList();
    void onClickAddEstateManager();
    void onClickRemoveEstateManager();
    void onAllowedSearchEdit(const std::string& search_string);
    void onAllowedGroupsSearchEdit(const std::string& search_string);
    void onBannedSearchEdit(const std::string& search_string);

    // Group picker callback is different, can't use core methods below
    bool addAllowedGroup(const LLSD& notification, const LLSD& response);
    void addAllowedGroup2(LLUUID id);

    // Core methods for all above add/remove button clicks
    static void accessAddCore(U32 operation_flag, const std::string& dialog_name);
    static bool accessAddCore2(const LLSD& notification, const LLSD& response);
    static void accessAddCore3(const uuid_vec_t& ids, std::vector<LLAvatarName> names, void* data);

    static void accessRemoveCore(U32 operation_flag, const std::string& dialog_name, const std::string& list_ctrl_name);
    static bool accessRemoveCore2(const LLSD& notification, const LLSD& response);

    // used for both add and remove operations
    static bool accessCoreConfirm(const LLSD& notification, const LLSD& response);

    // Send the actual EstateOwnerRequest "estateaccessdelta" message
    // <FS:Ansariel> Moved to public
    //static void sendEstateAccessDelta(U32 flags, const LLUUID& agent_id);

    static void requestEstateGetAccessCoro(std::string url);

    void searchAgent(LLNameListCtrl* listCtrl, const std::string& search_string);
    void copyListToClipboard(std::string list_name);

    bool mPendingUpdate;
    bool mCtrlsEnabled;
};

#endif
