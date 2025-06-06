/**
 * @file llvoavatarself.h
 * @brief Declaration of LLVOAvatar class which is a derivation fo
 * LLViewerObject
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

#ifndef LL_LLVOAVATARSELF_H
#define LL_LLVOAVATARSELF_H

#include "llviewertexture.h"
#include "llvoavatar.h"
#include <map>
#include "lleventcoro.h"
#include "llcoros.h"

struct LocalTextureData;
class LLInventoryCallback;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LLVOAvatarSelf
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class LLVOAvatarSelf :
    public LLVOAvatar
{
    LOG_CLASS(LLVOAvatarSelf);

/********************************************************************************
 **                                                                            **
 **                    INITIALIZATION
 **/

public:
    LLVOAvatarSelf(const LLUUID &id, const LLPCode pcode, LLViewerRegion *regionp);
    virtual                 ~LLVOAvatarSelf();
    virtual void            markDead();
    virtual void            initInstance(); // Called after construction to initialize the class.
    void                    cleanup();
protected:
    /*virtual*/ bool        loadAvatar();
    bool                    loadAvatarSelf();
    bool                    buildSkeletonSelf(const LLAvatarSkeletonInfo *info);
    bool                    buildMenus();

/**                    Initialization
 **                                                                            **
 *******************************************************************************/

/********************************************************************************
 **                                                                            **
 **                    INHERITED
 **/

    //--------------------------------------------------------------------
    // LLViewerObject interface and related
    //--------------------------------------------------------------------
public:
    boost::signals2::connection                   mRegionChangedSlot;

    void                    onSimulatorFeaturesReceived(const LLUUID& region_id);
    /*virtual*/ void        updateRegion(LLViewerRegion *regionp);
    /*virtual*/ void        idleUpdate(LLAgent &agent, const F64 &time);

    //--------------------------------------------------------------------
    // LLCharacter interface and related
    //--------------------------------------------------------------------
public:
    /*virtual*/ bool        hasMotionFromSource(const LLUUID& source_id);
    /*virtual*/ void        stopMotionFromSource(const LLUUID& source_id);
    /*virtual*/ void        requestStopMotion(LLMotion* motion);

    //<FS:Ansariel> Joint-lookup improvements
//  /*virtual*/ LLJoint*    getJoint(const std::string &name);
    /*virtual*/ LLJoint*    getJoint(std::string_view name);

    /*virtual*/ void renderJoints();

    // <FS:Ansariel> [Legacy Bake]
    ///*virtual*/ bool setVisualParamWeight(const LLVisualParam *which_param, F32 weight);
    ///*virtual*/ bool setVisualParamWeight(const char* param_name, F32 weight);
    ///*virtual*/ bool setVisualParamWeight(S32 index, F32 weight);
    /*virtual*/ bool setVisualParamWeight(const LLVisualParam *which_param, F32 weight, bool upload_bake = false);
    /*virtual*/ bool setVisualParamWeight(const char* param_name, F32 weight, bool upload_bake = false);
    /*virtual*/ bool setVisualParamWeight(S32 index, F32 weight, bool upload_bake = false);
    // </FS:Ansariel> [Legacy Bake]
    /*virtual*/ void updateVisualParams();
    void writeWearablesToAvatar();
    /*virtual*/ void idleUpdateAppearanceAnimation();

    // <FS:Ansariel> [Legacy Bake]
    /*virtual*/ U32  processUpdateMessage(LLMessageSystem *mesgsys,
                                                     void **user_data,
                                                     U32 block_num,
                                                     const EObjectUpdateType update_type,
                                                     LLDataPacker *dp);
    // </FS:Ansariel> [Legacy Bake]

private:
    // helper function. Passed in param is assumed to be in avatar's parameter list.
    // <FS:Ansariel> [Legacy Bake]
    //bool setParamWeight(const LLViewerVisualParam *param, F32 weight);
    bool setParamWeight(const LLViewerVisualParam *param, F32 weight, bool upload_bake = false);

    std::mutex          mJointMapMutex; // getJoint gets used from mesh thread

/********************************************************************************
 **                                                                            **
 **                    STATE
 **/

public:
    /*virtual*/ bool    isSelf() const { return true; }
        virtual bool    isBuddy() const { return false; }
    /*virtual*/ bool    isValid() const; // use isAgentAvatarValid, it's fuller

    //--------------------------------------------------------------------
    // Updates
    //--------------------------------------------------------------------
public:
    /*virtual*/ bool    updateCharacter(LLAgent &agent);
    /*virtual*/ void    idleUpdateTractorBeam();
    bool                checkStuckAppearance();

    //--------------------------------------------------------------------
    // Loading state
    //--------------------------------------------------------------------
public:
    /*virtual*/ bool    getIsCloud() const;

    //--------------------------------------------------------------------
    // Region state
    //--------------------------------------------------------------------
    void            resetRegionCrossingTimer()  { mRegionCrossingTimer.reset(); }
    // <FS:Ansariel> FIRE-12004: Attachments getting lost on TP
    void            setIsCrossingRegion(bool is_crossing) { mIsCrossingRegion = is_crossing; }
    bool            isCrossingRegion() const { return mIsCrossingRegion; }
    // </FS:Ansariel>

private:
    U64             mLastRegionHandle;
    LLFrameTimer    mRegionCrossingTimer;
    S32             mRegionCrossingCount;

    // <FS:Ansariel> FIRE-12004: Attachments getting lost on TP
    bool            mIsCrossingRegion;

/**                    State
 **                                                                            **
 *******************************************************************************/

/********************************************************************************
 **                                                                            **
 **                    RENDERING
 **/

    //--------------------------------------------------------------------
    // Render beam
    //--------------------------------------------------------------------
protected:
    bool        needsRenderBeam();
private:
    LLPointer<LLHUDEffectSpiral> mBeam;
    LLFrameTimer mBeamTimer;

    //--------------------------------------------------------------------
    // LLVOAvatar Constants
    //--------------------------------------------------------------------
public:
    /*virtual*/ LLViewerTexture::EBoostLevel    getAvatarBoostLevel() const { return LLGLTexture::BOOST_AVATAR_SELF; }
    /*virtual*/ LLViewerTexture::EBoostLevel    getAvatarBakedBoostLevel() const { return LLGLTexture::BOOST_AVATAR_BAKED_SELF; }
    /*virtual*/ S32                         getTexImageSize() const { return LLVOAvatar::getTexImageSize()*4; }

/**                    Rendering
 **                                                                            **
 *******************************************************************************/

/********************************************************************************
 **                                                                            **
 **                    TEXTURES
 **/

    //--------------------------------------------------------------------
    // Loading status
    //--------------------------------------------------------------------
public:
    // <FS:Ansariel> [Legacy Bake]
    /*virtual*/ bool    hasPendingBakedUploads() const;
    S32                 getLocalDiscardLevel(LLAvatarAppearanceDefines::ETextureIndex type, U32 index) const;
    bool                areTexturesCurrent() const;
    bool                isLocalTextureDataAvailable(const LLViewerTexLayerSet* layerset) const;
    bool                isLocalTextureDataFinal(const LLViewerTexLayerSet* layerset) const;
    // <FS:Ansariel> [Legacy Bake]
    bool                isBakedTextureFinal(const LLAvatarAppearanceDefines::EBakedTextureIndex index) const;
    // If you want to check all textures of a given type, pass gAgentWearables.getWearableCount() for index
    /*virtual*/ bool    isTextureDefined(LLAvatarAppearanceDefines::ETextureIndex type, U32 index) const;
    /*virtual*/ bool    isTextureVisible(LLAvatarAppearanceDefines::ETextureIndex type, U32 index = 0) const;
    /*virtual*/ bool    isTextureVisible(LLAvatarAppearanceDefines::ETextureIndex type, LLViewerWearable *wearable) const;


    //--------------------------------------------------------------------
    // Local Textures
    //--------------------------------------------------------------------
public:
    bool                getLocalTextureGL(LLAvatarAppearanceDefines::ETextureIndex type, LLViewerTexture** image_gl_pp, U32 index) const;
    LLViewerFetchedTexture* getLocalTextureGL(LLAvatarAppearanceDefines::ETextureIndex type, U32 index) const;
    const LLUUID&       getLocalTextureID(LLAvatarAppearanceDefines::ETextureIndex type, U32 index) const;
    void                setLocalTextureTE(U8 te, LLViewerTexture* image, U32 index);
    /*virtual*/ void    setLocalTexture(LLAvatarAppearanceDefines::ETextureIndex type, LLViewerTexture* tex, bool baked_version_exits, U32 index);
protected:
    /*virtual*/ void    setBakedReady(LLAvatarAppearanceDefines::ETextureIndex type, bool baked_version_exists, U32 index);
    void                localTextureLoaded(bool succcess, LLViewerFetchedTexture *src_vi, LLImageRaw* src, LLImageRaw* aux_src, S32 discard_level, bool final, void* userdata);
    void                getLocalTextureByteCount(S32* gl_byte_count) const;
    /*virtual*/ void    addLocalTextureStats(LLAvatarAppearanceDefines::ETextureIndex i, LLViewerFetchedTexture* imagep, F32 texel_area_ratio, bool rendered, bool covered_by_baked);
    LLLocalTextureObject* getLocalTextureObject(LLAvatarAppearanceDefines::ETextureIndex i, U32 index) const;

private:
    static void         onLocalTextureLoaded(bool succcess, LLViewerFetchedTexture *src_vi, LLImageRaw* src, LLImageRaw* aux_src, S32 discard_level, bool final, void* userdata);

    /*virtual*/ void    setImage(const U8 te, LLViewerTexture *imagep, const U32 index);
    /*virtual*/ LLViewerTexture* getImage(const U8 te, const U32 index) const;


    //--------------------------------------------------------------------
    // Baked textures
    //--------------------------------------------------------------------
public:
    LLAvatarAppearanceDefines::ETextureIndex getBakedTE(const LLViewerTexLayerSet* layerset ) const;
    //-- SUNSHINE CLEANUP - dead? or update to just call request appearance update?
    void                forceBakeAllTextures(bool slam_for_debug = false);
    // <FS:Ansariel> [Legacy Bake]
    void                setNewBakedTexture(LLAvatarAppearanceDefines::EBakedTextureIndex i, const LLUUID &uuid);
    void                setNewBakedTexture(LLAvatarAppearanceDefines::ETextureIndex i, const LLUUID& uuid);
    void                setCachedBakedTexture(LLAvatarAppearanceDefines::ETextureIndex i, const LLUUID& uuid);
    static void         processRebakeAvatarTextures(LLMessageSystem* msg, void**);
    // </FS:Ansariel> [Legacy Bake]
protected:
    /*virtual*/ void    removeMissingBakedTextures();

    //--------------------------------------------------------------------
    // Layers
    //--------------------------------------------------------------------
public:
    // <FS:Ansariel> [Legacy Bake]
    void                requestLayerSetUploads();
    void                requestLayerSetUpload(LLAvatarAppearanceDefines::EBakedTextureIndex i);
    // </FS:Ansariel> [Legacy Bake]
    void                requestLayerSetUpdate(LLAvatarAppearanceDefines::ETextureIndex i);
    LLViewerTexLayerSet* getLayerSet(LLAvatarAppearanceDefines::EBakedTextureIndex baked_index) const;
    LLViewerTexLayerSet* getLayerSet(LLAvatarAppearanceDefines::ETextureIndex index) const;


    //--------------------------------------------------------------------
    // Composites
    //--------------------------------------------------------------------
public:
    // <FS:Ansariel> [Legacy Bake]
    ///* virtual */ void    invalidateComposite(LLTexLayerSet* layerset);
    /* virtual */ void  invalidateComposite(LLTexLayerSet* layerset, bool upload_result);
    /* virtual */ void  invalidateAll();
    /* virtual */ void  setCompositeUpdatesEnabled(bool b); // only works for self
    /* virtual */ void  setCompositeUpdatesEnabled(U32 index, bool b);
    /* virtual */ bool  isCompositeUpdateEnabled(U32 index);
    void                setupComposites();
    void                updateComposites();

    const LLUUID&       grabBakedTexture(LLAvatarAppearanceDefines::EBakedTextureIndex baked_index) const;
    bool                canGrabBakedTexture(LLAvatarAppearanceDefines::EBakedTextureIndex baked_index) const;


    //--------------------------------------------------------------------
    // Scratch textures (used for compositing)
    //--------------------------------------------------------------------
public:
    static void     deleteScratchTextures();
private:
    static S32Bytes sScratchTexBytes;
    static std::map< LLGLenum, LLGLuint*> sScratchTexNames;

/**                    Textures
 **                                                                            **
 *******************************************************************************/

/********************************************************************************
 **                                                                            **
 **                    MESHES
 **/
protected:
    /*virtual*/ void   restoreMeshData();

/**                    Meshes
 **                                                                            **
 *******************************************************************************/

/********************************************************************************
 **                                                                            **
 **                    WEARABLES
 **/

public:
    // <FS:Ansariel> [Legacy Bake]
    //void              wearableUpdated(LLWearableType::EType type);
    void                wearableUpdated(LLWearableType::EType type, bool upload_result);
protected:
    U32 getNumWearables(LLAvatarAppearanceDefines::ETextureIndex i) const;

    //--------------------------------------------------------------------
    // Attachments
    //--------------------------------------------------------------------
public:
    void                updateAttachmentVisibility(U32 camera_mode);
    bool                isWearingAttachment(const LLUUID& inv_item_id) const;
    LLViewerObject*     getWornAttachment(const LLUUID& inv_item_id);
    bool                getAttachedPointName(const LLUUID& inv_item_id, std::string& name) const;
// [RLVa:KB] - Checked: 2009-12-18 (RLVa-1.1.0i) | Added: RLVa-1.1.0i
    LLViewerJointAttachment* getWornAttachmentPoint(const LLUUID& inv_item_id) const;
// [/RLVa:KB]
    /*virtual*/ const LLViewerJointAttachment *attachObject(LLViewerObject *viewer_object);
    /*virtual*/ bool    detachObject(LLViewerObject *viewer_object);
    static bool         detachAttachmentIntoInventory(const LLUUID& item_id);

    bool hasAttachmentsInTrash();

// [RLVa:KB] - Checked: 2012-07-28 (RLVa-1.4.7)
    enum EAttachAction { ACTION_ATTACH, ACTION_DETACH };
    typedef boost::signals2::signal<void (LLViewerObject*, const LLViewerJointAttachment*, EAttachAction)> attachment_signal_t;
    boost::signals2::connection setAttachmentCallback(const attachment_signal_t::slot_type& cb);
// [/RLVa:KB]
// [RLVa:KB] - Checked: 2012-07-28 (RLVa-1.4.7)
    attachment_signal_t* mAttachmentSignal;
// [/RLVa:KB]
    //--------------------------------------------------------------------
    // HUDs
    //--------------------------------------------------------------------
private:
    LLViewerJoint*      mScreenp; // special purpose joint for HUD attachments

/**                    Attachments
 **                                                                            **
 *******************************************************************************/

/********************************************************************************
 **                                                                            **
 **                    APPEARANCE
 **/

public:
    static void     onCustomizeStart(bool disable_camera_switch = false);
    static void     onCustomizeEnd(bool disable_camera_switch = false);
    LLPointer<LLInventoryCallback> mEndCustomizeCallback;

    //--------------------------------------------------------------------
    // Visibility
    //--------------------------------------------------------------------

    /* virtual */ bool shouldRenderRigged() const;

public:
    bool            sendAppearanceMessage(LLMessageSystem *mesgsys) const;

    // -- care and feeding of hover height.
    void            setHoverIfRegionEnabled();
    void            sendHoverHeight() const;
    /*virtual*/ void setHoverOffset(const LLVector3& hover_offset, bool send_update=true);

private:
    mutable LLVector3 mLastHoverOffsetSent;

// [RLVa:KB] - Checked: 2013-03-03 (RLVa-1.4.8)
protected:
    /*virtual*/ F32 getAvatarOffset() /*const*/;
// [/RLVa:KB]

/**                    Appearance
 **                                                                            **
 *******************************************************************************/

/********************************************************************************
 **                                                                            **
 **                    DIAGNOSTICS
 **/

    //--------------------------------------------------------------------
    // General
    //--------------------------------------------------------------------
public:
    static void     dumpTotalLocalTextureByteCount();
    void            dumpLocalTextures() const;
    static void     dumpScratchTextureByteCount();
    void            dumpWearableInfo(LLAPRFile& outfile);

    //--------------------------------------------------------------------
    // Avatar Rez Metrics
    //--------------------------------------------------------------------
public:
    struct LLAvatarTexData
    {
        LLAvatarTexData(const LLUUID& id, LLAvatarAppearanceDefines::ETextureIndex index) :
            mAvatarID(id),
            mIndex(index)
        {}
        LLUUID          mAvatarID;
        LLAvatarAppearanceDefines::ETextureIndex    mIndex;
    };

    LLTimer                 mTimeSinceLastRezMessage;
    bool                    updateAvatarRezMetrics(bool force_send);

    std::vector<LLSD>       mPendingTimerRecords;
    void                    addMetricsTimerRecord(const LLSD& record);

    void                    debugWearablesLoaded() { mDebugTimeWearablesLoaded = mDebugSelfLoadTimer.getElapsedTimeF32(); }
    void                    debugAvatarVisible() { mDebugTimeAvatarVisible = mDebugSelfLoadTimer.getElapsedTimeF32(); }
    void                    outputRezDiagnostics() const;
    void                    outputRezTiming(const std::string& msg) const;
    void                    reportAvatarRezTime() const;
    void                    debugBakedTextureUpload(LLAvatarAppearanceDefines::EBakedTextureIndex index, bool finished);
    static void             debugOnTimingLocalTexLoaded(bool success, LLViewerFetchedTexture *src_vi, LLImageRaw* src, LLImageRaw* aux_src, S32 discard_level, bool final, void* userdata);

    bool                    isAllLocalTextureDataFinal() const;

    const LLViewerTexLayerSet*  debugGetLayerSet(LLAvatarAppearanceDefines::EBakedTextureIndex index) const { return (LLViewerTexLayerSet*)(mBakedTextureDatas[index].mTexLayerSet); }
    const std::string       verboseDebugDumpLocalTextureDataInfo(const LLViewerTexLayerSet* layerset) const; // Lists out state of this particular baked texture layer
    void                    dumpAllTextures() const;
    const std::string       debugDumpLocalTextureDataInfo(const LLViewerTexLayerSet* layerset) const; // Lists out state of this particular baked texture layer
    const std::string       debugDumpAllLocalTextureDataInfo() const; // Lists out which baked textures are at highest LOD
    void                    sendViewerAppearanceChangeMetrics(); // send data associated with completing a change.
    // <FS:Ansariel> [Legacy Bake]
    void                    checkForUnsupportedServerBakeAppearance();
private:
    LLFrameTimer            mDebugSelfLoadTimer;
    F32                     mDebugTimeWearablesLoaded;
    F32                     mDebugTimeAvatarVisible;
    F32                     mDebugTextureLoadTimes[LLAvatarAppearanceDefines::TEX_NUM_INDICES][MAX_DISCARD_LEVEL+1]; // load time for each texture at each discard level
    F32                     mDebugBakedTextureTimes[LLAvatarAppearanceDefines::BAKED_NUM_INDICES][2]; // time to start upload and finish upload of each baked texture
    void                    debugTimingLocalTexLoaded(bool success, LLViewerFetchedTexture *src_vi, LLImageRaw* src, LLImageRaw* aux_src, S32 discard_level, bool final, void* userdata);
    void                    checkBOMRebakeRequired();
    void                    appearanceChangeMetricsCoro(std::string url);
    bool                    mInitialMetric;
    S32                     mMetricSequence;

/**                    Diagnostics
 **                                                                            **
 *******************************************************************************/

};

extern LLPointer<LLVOAvatarSelf> gAgentAvatarp;

bool isAgentAvatarValid();

void selfStartPhase(const std::string& phase_name);
void selfStopPhase(const std::string& phase_name, bool err_check = true);
void selfClearPhases();

#endif // LL_VO_AVATARSELF_H
