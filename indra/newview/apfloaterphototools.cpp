/**
 * @file apfloaterphototools.cpp
 * @brief Phototools floater
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 *
 * Copyright (C) 2025, William Weaver (paperwork) @ Second Life
 * Copyright (C) 2011, WoLf Loonie @ Second Life
 * Copyright (C) 2013, Zi Ree @ Second Life
 * Copyright (C) 2013, Ansariel Hiller @ Second Life
 * Copyright (C) 2013, Cinder Biscuits @ Me too
 
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

#include "apfloaterphototools.h"

#include "llcheckboxctrl.h"
#include "llcombobox.h"
#include "llfloaterpreference.h"
#include "llinventoryfunctions.h"
#include "llnotificationsutil.h"
#include "llsliderctrl.h"
#include "llspinctrl.h"
#include "lltrans.h"
#include "llviewercontrol.h"
#include "llviewernetwork.h" 
#include "llviewerwindow.h"

#include "llquaternion.h"
#include "llvirtualtrackball.h"
#include "llsky.h"
#include "llsettingssky.h"

#include "llcolorswatch.h"
#include "lltexturectrl.h"
#include "llenvironment.h"
#include "pipeline.h"

#include "llfloaterreg.h"

#include "llfeaturemanager.h"

#include "llagentcamera.h"
#include "llbutton.h"
#include "llslider.h"
#include "lljoystickbutton.h"
#include "llfirstuse.h"

#include "llpresetsmanager.h"

#include "fscommon.h"
#include "llviewerjoystick.h"
#include "llworld.h"
#include "lltoolmgr.h"
#include "lltoolfocus.h"

#include "llagent.h"

#include "llmath.h"

#include <string>
#include <iomanip>
#include <sstream>

namespace
{
    // Atmospheric Colors
    const std::string FIELD_SKY_AMBIENT_LIGHT("ambient_light");
    const std::string FIELD_SKY_BLUE_HORIZON("blue_horizon");
    const std::string FIELD_SKY_BLUE_DENSITY("blue_density");

    // Atmosphere Settings
    const std::string   FIELD_SKY_HAZE_HORIZON("haze_horizon");
    const std::string   FIELD_SKY_HAZE_DENSITY("haze_density");
    const std::string   FIELD_SKY_DENSITY_MULTIP("density_multip");
    const std::string   FIELD_SKY_DISTANCE_MULTIP("distance_multip");
    const std::string   FIELD_SKY_MAX_ALT("max_alt");
    const std::string FIELD_SKY_SCENE_GAMMA("scene_gamma");
    // void updateGammaLabel();                    // FEA (I think this is UI for Brightness)
    const std::string FIELD_REFLECTION_PROBE_AMBIANCE("probe_ambiance");

    // Rainbow and Halo Settings
    const std::string   FIELD_SKY_DENSITY_MOISTURE_LEVEL("moisture_level");
    const std::string   FIELD_SKY_DENSITY_DROPLET_RADIUS("droplet_radius");
    const std::string   FIELD_SKY_DENSITY_ICE_LEVEL("ice_level");

    // Cloud Settings
    const std::string FIELD_SKY_CLOUD_COLOR("cloud_color");
    const std::string FIELD_SKY_CLOUD_COVERAGE("cloud_coverage");
    const std::string FIELD_SKY_CLOUD_SCALE("cloud_scale");
    const std::string   FIELD_SKY_CLOUD_VARIANCE("cloud_variance");
    const std::string   FIELD_SKY_CLOUD_DENSITY_X("cloud_density_x");
    const std::string   FIELD_SKY_CLOUD_DENSITY_Y("cloud_density_y");
    const std::string   FIELD_SKY_CLOUD_DENSITY_D("cloud_density_d");
    const std::string   FIELD_SKY_CLOUD_DETAIL_X("cloud_detail_x");
    const std::string   FIELD_SKY_CLOUD_DETAIL_Y("cloud_detail_y");
    const std::string   FIELD_SKY_CLOUD_DETAIL_D("cloud_detail_d");
    const std::string   FIELD_SKY_CLOUD_SCROLL_XY("cloud_scroll_xy");
    const std::string FIELD_SKY_CLOUD_MAP("cloud_map");

    // Sun and Moon Colors
    const std::string FIELD_SKY_SUN_COLOR("sun_moon_color");

    // Sun and Stars Settings
    const std::string   FIELD_SKY_SUN_IMAGE("sun_image");
    const std::string FIELD_SKY_GLOW_FOCUS("glow_focus");
    const std::string FIELD_SKY_GLOW_SIZE("glow_size");
    const std::string FIELD_SKY_STAR_BRIGHTNESS("star_brightness");
    const std::string FIELD_SKY_SUN_SCALE("sun_scale");
    const std::string FIELD_SKY_SUN_AZIMUTH("sun_azimuth");
    const std::string FIELD_SKY_SUN_ELEVATION("sun_elevation");
    
    // Moon Settings
    const std::string   FIELD_SKY_MOON_IMAGE("moon_image");
    const std::string   FIELD_SKY_MOON_SCALE("moon_scale");
    const std::string   FIELD_SKY_MOON_BRIGHTNESS("moon_brightness");
    const std::string FIELD_SKY_MOON_AZIMUTH("moon_azimuth");
    const std::string FIELD_SKY_MOON_ELEVATION("moon_elevation");

    const F32 SLIDER_SCALE_SUN_AMBIENT(3.0f);
    const F32 SLIDER_SCALE_BLUE_HORIZON_DENSITY(2.0f);
    const F32 SLIDER_SCALE_GLOW_R(20.0f);
    const F32 SLIDER_SCALE_GLOW_B(-5.0f);
    const F32 SLIDER_SCALE_DENSITY_MULTIPLIER(0.001f);

    const std::string   FIELD_WATER_FOG_COLOR("water_fog_color");
    const std::string   FIELD_WATER_FOG_DENSITY("water_fog_density");
    const std::string   FIELD_WATER_UNDERWATER_MOD("water_underwater_mod");
    const std::string   FIELD_WATER_NORMAL_MAP("water_normal_map");

    const std::string   FIELD_WATER_WAVE1_XY("water_wave1_xy");
    const std::string   FIELD_WATER_WAVE2_XY("water_wave2_xy");

    const std::string   FIELD_WATER_NORMAL_SCALE_X("water_normal_scale_x");
    const std::string   FIELD_WATER_NORMAL_SCALE_Y("water_normal_scale_y");
    const std::string   FIELD_WATER_NORMAL_SCALE_Z("water_normal_scale_z");

    const std::string   FIELD_WATER_FRESNEL_SCALE("water_fresnel_scale");
    const std::string   FIELD_WATER_FRESNEL_OFFSET("water_fresnel_offset");

    const std::string   FIELD_WATER_SCALE_ABOVE("water_scale_above");
    const std::string   FIELD_WATER_SCALE_BELOW("water_scale_below");
    const std::string   FIELD_WATER_BLUR_MULTIP("water_blur_multip");

    // const S32 FLOATER_ENVIRONMENT_UPDATE(-2);
    // const std::string FIELD_WATER_NORMAL_MAP("water_normal_map");
    // const std::string FIELD_SKY_SUN_ROTATION("sun_rotation");
    // const std::string FIELD_SKY_MOON_ROTATION("moon_rotation");
    // const std::string BTN_RESET("btn_reset");

    const F32 NUDGE_TIME = 0.25f;       // in seconds
    const F32 ORBIT_NUDGE_RATE = 0.05f; // fraction of normal speed

}

static void activate_camera_tool() // Added static
{
    // Make sure LLToolCamera::getInstance() exists and returns the correct tool instance
    LLTool* cam_tool = LLToolCamera::getInstance();
    if (cam_tool)
    {
        LLToolMgr::getInstance()->setTransientTool(cam_tool);
        LL_DEBUGS("PhotoToolsCamera") << "Activated LLToolCamera" << LL_ENDL;
    }
    else
    {
        LL_WARNS("PhotoToolsCamera") << "Could not get LLToolCamera instance!" << LL_ENDL;
    }
}

static void clear_camera_tool() // Added static
{
    LLToolMgr* tool_mgr = LLToolMgr::getInstance();
    LLTool* cam_tool = LLToolCamera::getInstance();
    // Clear only if the current transient tool IS the camera tool
    if (cam_tool && tool_mgr->usingTransientTool() &&
        tool_mgr->getCurrentTool() == cam_tool)
    {
        tool_mgr->clearTransientTool();
        LL_DEBUGS("PhotoToolsCamera") << "Cleared LLToolCamera" << LL_ENDL;
    }
}

class APSettingsCollector : public LLInventoryCollectFunctor
{
public:
    APSettingsCollector()
    {
        // No folder filtering in the collector itself anymore
    }

    virtual ~APSettingsCollector() {}

    bool operator()(LLInventoryCategory* cat, LLInventoryItem* item)
    {
        if (item && item->getType() == LLAssetType::AT_SETTINGS)
        {
            // <AP:WW> Collect all settings types now, filtering in loadPresets()
            if (mSeen.find(item->getAssetUUID()) == mSeen.end())
            {
                mSeen.insert(item->getAssetUUID());
                return true; // Include the item if it's a settings type
            }
        }
        return false; // Exclude the item otherwise
    }

protected:
    std::set<LLUUID> mSeen;
};

APFloaterPhototools::APFloaterPhototools(const LLSD& key)
:   LLFloater(key),

    mCamRotateStick(nullptr),
    mBtnRollLeft(nullptr),
    mBtnRollRight(nullptr),
    mBtnZoomPlus(nullptr),
    mBtnZoomMinus(nullptr),
    mSliderZoomDistance(nullptr),
    mCamTrackStick(nullptr),
    mBtnCamPresetsView(nullptr),
    mBtnCamPanView(nullptr),
    mBtnCamModesView(nullptr),
    mPanelPresetViews(nullptr),
    mPanelCameraModes(nullptr),
    mPanelZoomControls(nullptr),

    mEnvChangedConnection()

{

    mCommitCallbackRegistrar.add("CameraPresets.ChangeView", boost::bind(&APFloaterPhototools::onClickCameraItemHandler, this, _2));
    mCommitCallbackRegistrar.add("CameraPresets.ShowPresetsList", boost::bind(&APFloaterPhototools::onShowCameraPresetsFloater, this));

    mCommitCallbackRegistrar.add("Camera.StoreView01", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 1));
    mCommitCallbackRegistrar.add("Camera.StoreView02", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 2));
    mCommitCallbackRegistrar.add("Camera.StoreView03", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 3));
    mCommitCallbackRegistrar.add("Camera.StoreView04", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 4));
    mCommitCallbackRegistrar.add("Camera.StoreView05", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 5));
    mCommitCallbackRegistrar.add("Camera.StoreView06", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 6));
    mCommitCallbackRegistrar.add("Camera.StoreView07", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 7));
    mCommitCallbackRegistrar.add("Camera.StoreView08", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 8));
    mCommitCallbackRegistrar.add("Camera.StoreView09", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 9));
    mCommitCallbackRegistrar.add("Camera.StoreView10", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 10));
    mCommitCallbackRegistrar.add("Camera.StoreView11", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 11));
    mCommitCallbackRegistrar.add("Camera.StoreView12", boost::bind(&APFloaterPhototools::onStoreCameraView, this, 12));

    mCommitCallbackRegistrar.add("Camera.LoadView01", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 1));
    mCommitCallbackRegistrar.add("Camera.LoadView02", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 2));
    mCommitCallbackRegistrar.add("Camera.LoadView03", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 3));
    mCommitCallbackRegistrar.add("Camera.LoadView04", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 4));
    mCommitCallbackRegistrar.add("Camera.LoadView05", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 5));
    mCommitCallbackRegistrar.add("Camera.LoadView06", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 6));
    mCommitCallbackRegistrar.add("Camera.LoadView07", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 7));
    mCommitCallbackRegistrar.add("Camera.LoadView08", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 8));
    mCommitCallbackRegistrar.add("Camera.LoadView09", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 9));
    mCommitCallbackRegistrar.add("Camera.LoadView10", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 10));
    mCommitCallbackRegistrar.add("Camera.LoadView11", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 11));
    mCommitCallbackRegistrar.add("Camera.LoadView12", boost::bind(&APFloaterPhototools::onLoadCameraView, this, 12));

    mCommitCallbackRegistrar.add("Camera.StoreFlycamView01", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 1));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView02", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 2));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView03", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 3));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView04", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 4));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView05", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 5));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView06", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 6));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView07", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 7));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView08", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 8));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView09", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 9));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView10", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 10));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView11", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 11));
    mCommitCallbackRegistrar.add("Camera.StoreFlycamView12", boost::bind(&APFloaterPhototools::onStoreFlycamView, this, 12));

    mCommitCallbackRegistrar.add("Camera.LoadFlycamView01", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 1));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView02", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 2));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView03", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 3));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView04", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 4));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView05", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 5));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView06", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 6));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView07", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 7));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView08", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 8));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView09", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 9));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView10", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 10));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView11", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 11));
    mCommitCallbackRegistrar.add("Camera.LoadFlycamView12", boost::bind(&APFloaterPhototools::onLoadFlycamView, this, 12));

}

APFloaterPhototools::~APFloaterPhototools()
{
    if (mEnvChangedConnection.connected())
    {
        mEnvChangedConnection.disconnect();
    }
}

void APFloaterPhototools::onOpen(const LLSD& key)
{
    loadPresets();
    setSelectedEnvironment();
    LLAvatarComplexityControls::setIndirectMaxNonImpostors();
    LLAvatarComplexityControls::setIndirectMaxArc();
    LLAvatarComplexityControls::setText(gSavedSettings.getU32("RenderAvatarMaxComplexity"), mMaxComplexityLabel);
    refreshAvatarComplexityControlsFromSetting(LLSD());

    refreshSettings();             // Refresh settings-linked controls
    refreshCameraControls();       // <<<<<<<<<< ADDED: Refresh camera-state controls
    refreshColorBalanceControls(); // Refresh CB controls
    refreshSky();                  // Refresh sky controls
    updateCameraItemsSelection();

    // if (!mLiveSky)
    // {
    //    LLEnvironment::instance().saveBeaconsState();
    // }
    // captureCurrentEnvironment();

    // mEnvChangedConnection = LLEnvironment::instance().setEnvironmentChanged([this](LLEnvironment::EnvSelection_t env, S32 version){ onEnvironmentUpdated(env, version); });

    // mEnvChangedConnection = LLEnvironment::instance().setEnvironmentChanged([this](LLEnvironment::EnvSelection_t env, S32 version){ setSelectedEnvironment(); });
    
    // HACK -- resume reflection map manager because "setEnvironmentChanged" may pause it (SL-20456)
    gPipeline.mReflectionMapManager.resume();

}

void APFloaterPhototools::initCallbacks()
{
    gSavedSettings.getControl("APPhototoolsShowOnlyMySettings")->getSignal()->connect(boost::bind(&APFloaterPhototools::onShowMySettingsChanged, this));
    
    getChild<LLUICtrl>("WLPresetsCombo")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeSkyPreset, this));
    getChild<LLUICtrl>("WLPrevPreset")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickSkyPrev, this));
    getChild<LLUICtrl>("WLNextPreset")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickSkyNext, this));

    getChild<LLUICtrl>("WaterPresetsCombo")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeWaterPreset, this));
    getChild<LLUICtrl>("WWPrevPreset")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickWaterPrev, this));
    getChild<LLUICtrl>("WWNextPreset")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickWaterNext, this));
    
    // getChild<LLUICtrl>("DCPresetsCombo")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeDayCyclePreset, this));
    // getChild<LLUICtrl>("DCPrevPreset")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickDayCyclePrev, this));
    // getChild<LLUICtrl>("DCNextPreset")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickDayCycleNext, this));

    gSavedSettings.getControl("RenderShadowSplitExponent")->getSignal()->connect(boost::bind(&APFloaterPhototools::refreshSettings, this));
    
    getChild<LLSlider>("SB_Shd_Clarity")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeRenderShadowSplitExponentSliderY, this));
    getChild<LLSpinCtrl>("S_Shd_Clarity")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeRenderShadowSplitExponentSpinnerY, this));
    getChild<LLButton>("Shd_Clarity_Reset")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetRenderShadowSplitExponentY, this));

    gSavedSettings.getControl("RenderShadowGaussian")->getSignal()->connect(boost::bind(&APFloaterPhototools::refreshSettings, this));
    
    getChild<LLSlider>("SB_Shd_Soften")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeRenderShadowGaussianSlider, this));
    getChild<LLSpinCtrl>("S_Shd_Soften")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeRenderShadowGaussianSpinner, this));
    getChild<LLButton>("Reset_Shd_Soften")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetRenderShadowGaussianX, this));

    getChild<LLSlider>("SB_AO_Soften")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeRenderShadowGaussianSlider, this));
    getChild<LLSpinCtrl>("S_AO_Soften")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeRenderShadowGaussianSpinner, this));
    getChild<LLButton>("Reset_AO_Soften")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetRenderShadowGaussianY, this));

    gSavedSettings.getControl("RenderSSAOEffect")->getSignal()->connect(boost::bind(&APFloaterPhototools::refreshSettings, this));

    getChild<LLSlider>("SB_Effect")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeRenderSSAOEffectSlider, this));
    getChild<LLSpinCtrl>("S_Effect")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeRenderSSAOEffectSpinner, this));
    getChild<LLButton>("Reset_Effect")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetRenderSSAOEffectX, this));

    getChild<LLSlider>("SB_Saturation")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeRenderSSAOEffectSliderY, this));
    getChild<LLSpinCtrl>("S_Saturation")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeRenderSSAOEffectSpinnerY, this));
    getChild<LLButton>("Reset_Saturation")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetRenderSSAOEffectY, this));

    // <AP:WW> REVISED START: Set Callbacks for ALL Luminance Weight Controls using getChild directly
    getChild<LLSlider>("SB_LumWeightR")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeLumWeightRSlider, this));
    getChild<LLSpinCtrl>("S_LumWeightR")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeLumWeightRSpinner, this));
    getChild<LLButton>("ResetLumWeightRBtn")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetLumWeightR, this));

    getChild<LLSlider>("SB_LumWeightG")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeLumWeightGSlider, this));
    getChild<LLSpinCtrl>("S_LumWeightG")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeLumWeightGSpinner, this));
    getChild<LLButton>("ResetLumWeightGBtn")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetLumWeightG, this));

    getChild<LLSlider>("SB_LumWeightB")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeLumWeightBSlider, this));
    getChild<LLSpinCtrl>("S_LumWeightB")->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeLumWeightBSpinner, this));
    getChild<LLButton>("ResetLumWeightBBtn")->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetLumWeightB, this));
    // <AP:WW> REVISED END: Set Callbacks for ALL Luminance Weight Controls

    // <AP:WW> CORRECTED START: Connect APLuminanceWeights signal to refreshSettings
    LLControlVariable* lum_weights_setting = gSavedSettings.getControl("APLuminanceWeights");
    if (lum_weights_setting)
    {
        lum_weights_setting->getSignal()->connect(boost::bind(&APFloaterPhototools::refreshSettings, this));
    }
    // <AP:WW> CORRECTED END: Connect signal

    // <AP:WW> ADD START: Connect Color Balance Callbacks
    // Connect the setting controlling the radio buttons to our handler
    LLControlVariable* cb_mode_setting = gSavedSettings.getControl("APColorBalanceMode");
    if (cb_mode_setting)
    {
        cb_mode_setting->getSignal()->connect(boost::bind(&APFloaterPhototools::onColorBalanceModeChanged, this));
        // Note: The APColorBalancePreserveLuma checkbox connects automatically via control_name
    }

    // Connect sliders (pass pointer to handler)
    if (mSliderCB_CyanRed)       mSliderCB_CyanRed->setCommitCallback(boost::bind(&APFloaterPhototools::onColorBalanceSliderChanged, this, _1, _2));
    if (mSliderCB_MagentaGreen)  mSliderCB_MagentaGreen->setCommitCallback(boost::bind(&APFloaterPhototools::onColorBalanceSliderChanged, this, _1, _2));
    if (mSliderCB_YellowBlue)    mSliderCB_YellowBlue->setCommitCallback(boost::bind(&APFloaterPhototools::onColorBalanceSliderChanged, this, _1, _2));

    // Connect spinners (pass pointer to handler)
    if (mSpinnerCB_CyanRed)      mSpinnerCB_CyanRed->setCommitCallback(boost::bind(&APFloaterPhototools::onColorBalanceSpinnerChanged, this, _1, _2));
    if (mSpinnerCB_MagentaGreen) mSpinnerCB_MagentaGreen->setCommitCallback(boost::bind(&APFloaterPhototools::onColorBalanceSpinnerChanged, this, _1, _2));
    if (mSpinnerCB_YellowBlue)   mSpinnerCB_YellowBlue->setCommitCallback(boost::bind(&APFloaterPhototools::onColorBalanceSpinnerChanged, this, _1, _2));

    // Connect reset buttons (pass pointer to handler)
    if (mResetBtnCB_CyanRed)     mResetBtnCB_CyanRed->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetColorBalance, this, _1, _2));
    if (mResetBtnCB_MagentaGreen)mResetBtnCB_MagentaGreen->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetColorBalance, this, _1, _2));
    if (mResetBtnCB_YellowBlue)  mResetBtnCB_YellowBlue->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetColorBalance, this, _1, _2));
    // <AP:WW> ADD END: Connect Color Balance Callbacks

    mMaxComplexitySlider->setCommitCallback(boost::bind(&APFloaterPhototools::updateMaxComplexity, this));
    // gSavedSettings.getControl("RenderAvatarMaxComplexity")->getCommitSignal()->connect(boost::bind(&APFloaterPhototools::updateMaxComplexityLabel, this, _2));
    LLControlVariable* complexity_setting = gSavedSettings.getControl("RenderAvatarMaxComplexity");
    if (complexity_setting)
    {
        complexity_setting->getCommitSignal()->connect(boost::bind(&APFloaterPhototools::refreshAvatarComplexityControlsFromSetting, this, _2));
    }
    gSavedSettings.getControl("IndirectMaxNonImpostors")->getCommitSignal()->connect(boost::bind(&APFloaterPhototools::updateMaxNonImpostors, this, _2));

    // <AP:WW> ADD START: Connect UI Scale control callbacks
    if (mUIScaleSlider)
    {
        mUIScaleSlider->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeUIScaleSlider, this));
    }
    if (mUIScaleSpinner)
    {
        mUIScaleSpinner->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeUIScaleSpinner, this));
    }
    if (mResetUIScaleBtn)
    {
        mResetUIScaleBtn->setCommitCallback(boost::bind(&APFloaterPhototools::onClickResetUIScale, this));
    }
    // <AP:WW> ADD END: Connect UI Scale control callbacks

    // <AP:WW> Bind Graphics Preset Combo callback
    if (mGraphicsPresetCombo)
    {
        mGraphicsPresetCombo->setCommitCallback(boost::bind(&APFloaterPhototools::onChangeQuality, this, _2));
    }

    // <AP:WW> START: Direct C++ Binding for Graphics Preset Buttons
    // Connect button commit signals directly to the member functions, explicitly passing the required parameter LLSD.
    LLSD graphic_param("graphic"); // Create the LLSD containing "graphic" once
    getChild<LLButton>("BTN_Graphics_Presets_Save")->setCommitCallback(boost::bind(&APFloaterPhototools::saveGraphicsPreset, this, graphic_param));
    getChild<LLButton>("BTN_Graphics_Presets_Load")->setCommitCallback(boost::bind(&APFloaterPhototools::loadGraphicsPreset, this, graphic_param));
    getChild<LLButton>("BTN_Graphics_Presets_Delete")->setCommitCallback(boost::bind(&APFloaterPhototools::deleteGraphicsPreset, this, graphic_param));
    // <AP:WW> END: Direct C++ Binding

    // >>>>>>>>>> START BLOCK: Connect Camera Control Callbacks <<<<<<<<<<
    // Note: Sliders like CameraAngle, ZoomTime, etc., and Checkboxes are linked via 'control_name' in XML.
    // Note: Reset buttons ('D') are linked via function="ResetControl" in XML.

    // Zoom Buttons (Connect commit callback. XML <mouse_held_callback> should trigger repeated calls if present)
    if (mBtnZoomPlus)
    {
        // Connect the single click / first press action
        mBtnZoomPlus->setCommitCallback(boost::bind(&APFloaterPhototools::onCameraZoomPlusHeldDown, this));
        // We CANNOT call setMouseHeldCallback. The continuous action relies on the XML's
        // <mouse_held_callback> tag or potentially the commit callback being fired repeatedly by the framework.
    }
    if (mBtnZoomMinus)
    {
        mBtnZoomMinus->setCommitCallback(boost::bind(&APFloaterPhototools::onCameraZoomMinusHeldDown, this));
    }

    // Zoom Distance Slider (Commit callback for when slider is released/changed)
    if (mSliderZoomDistance)
    {
        mSliderZoomDistance->setCommitCallback(boost::bind(&APFloaterPhototools::onCameraZoomSliderChanged, this));
    }

    // Roll Buttons (Connect commit callback. XML <mouse_held_callback> should trigger repeated calls if present)
    if (mBtnRollLeft)
    {
        mBtnRollLeft->setCommitCallback(boost::bind(&APFloaterPhototools::onCameraRollLeftHeldDown, this));
    }
    if (mBtnRollRight)
    {
        mBtnRollRight->setCommitCallback(boost::bind(&APFloaterPhototools::onCameraRollRightHeldDown, this));
    }

    // Joysticks - Optional connections if needed
    /*
    if (mCamRotateStick) { ... }
    if (mCamTrackStick) { ... }
    */

    // Connect Mode Buttons (To switch panel visibility)
    if (mBtnCamPresetsView)
    {
        mBtnCamPresetsView->setCommitCallback(boost::bind(&APFloaterPhototools::switchViews, this, CAMERA_CTRL_MODE_PRESETS));
    }
    if (mBtnCamModesView) // mBtnCamModesView points to 'avatarview_btn'
    {
        mBtnCamModesView->setCommitCallback(boost::bind(&APFloaterPhototools::switchViews, this, CAMERA_CTRL_MODE_MODES));
    }
    if (mBtnCamPanView)
    {
        mBtnCamPanView->setCommitCallback(boost::bind(&APFloaterPhototools::switchViews, this, CAMERA_CTRL_MODE_PAN));
    }
    // >>>>>>>>>> END BLOCK: Connect Camera Control Callbacks <<<<<<<<<<

    mEnvChangedConnection = LLEnvironment::instance().setEnvironmentChanged(
        [this](LLEnvironment::EnvSelection_t env, S32 version)
        {
            setSelectedEnvironment(); 
            refreshSky();
        });
}

bool APFloaterPhototools::postBuild()
{
    mShowMySettingsCheckbox = getChild<LLCheckBoxCtrl>("show_my_settings_checkbox"); 

    mWaterPresetsCombo = getChild<LLComboBox>("WaterPresetsCombo");
    mWLPresetsCombo = getChild<LLComboBox>("WLPresetsCombo");
    /// mDayCyclePresetsCombo = getChild<LLComboBox>("DCPresetsCombo");
 
    mSliderRenderShadowSplitExponentY = getChild<LLSlider>("SB_Shd_Clarity");
    mSpinnerRenderShadowSplitExponentY = getChild<LLSpinCtrl>("S_Shd_Clarity");

    mSliderRenderShadowGaussianX = getChild<LLSlider>("SB_Shd_Soften");
    mSpinnerRenderShadowGaussianX = getChild<LLSpinCtrl>("S_Shd_Soften");

    mSliderRenderShadowGaussianY = getChild<LLSlider>("SB_AO_Soften");
    mSpinnerRenderShadowGaussianY = getChild<LLSpinCtrl>("S_AO_Soften");

    mSliderRenderSSAOEffectX = getChild<LLSlider>("SB_Effect");
    mSpinnerRenderSSAOEffectX = getChild<LLSpinCtrl>("S_Effect");
    
    mSliderRenderSSAOEffectY = getChild<LLSlider>("SB_Saturation");
    mSpinnerRenderSSAOEffectY = getChild<LLSpinCtrl>("S_Saturation");
    
    mMaxComplexitySlider = getChild<LLSliderCtrl>("IndirectMaxComplexity2");
    mMaxComplexityLabel = getChild<LLTextBox>("IndirectMaxComplexityText2");

    // <AP:WW> START: Initialize Avatar Complexity slider and label in Phototools
    if (mMaxComplexitySlider && mMaxComplexityLabel) // Ensure pointers are valid
    {
        // 1. Update the 'IndirectMaxComplexity' setting. This is what the slider is bound to
        //    via its control_name. LLAvatarComplexityControls::setIndirectMaxArc() reads
        //    RenderAvatarMaxComplexity and calculates the appropriate IndirectMaxComplexity value.
        LLAvatarComplexityControls::setIndirectMaxArc(); 

        // 2. Update the text label directly.
        U32 current_complexity = gSavedSettings.getU32("RenderAvatarMaxComplexity");
        LLAvatarComplexityControls::setText(current_complexity, mMaxComplexityLabel);
    }
    // </AP:WW> END: Initialize Avatar Complexity slider and label

    // Atmospheric Colors
    getChild<LLUICtrl>(FIELD_SKY_AMBIENT_LIGHT)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onAmbientLightChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_BLUE_HORIZON)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onBlueHorizonChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_BLUE_DENSITY)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onBlueDensityChanged(); });

    // Atmosphere Settings
    getChild<LLUICtrl>(FIELD_SKY_HAZE_HORIZON)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onHazeHorizonChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_HAZE_DENSITY)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onHazeDensityChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_DENSITY_MULTIP)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onDensityMultipChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_DISTANCE_MULTIP)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onDistanceMultipChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_MAX_ALT)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onMaxAltChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_SCENE_GAMMA)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onSceneGammaChanged(); });
    getChild<LLUICtrl>(FIELD_REFLECTION_PROBE_AMBIANCE)->setCommitCallback([this](LLUICtrl*, const LLSD&) { onReflectionProbeAmbianceChanged(); });

    // Rainbow and Halo Settings
    getChild<LLUICtrl>(FIELD_SKY_DENSITY_MOISTURE_LEVEL)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onMoistureLevelChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_DENSITY_DROPLET_RADIUS)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onDropletRadiusChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_DENSITY_ICE_LEVEL)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onIceLevelChanged(); });

    // Cloud Settings
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_COLOR)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudColorChanged(); });
    
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_COVERAGE)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudCoverageChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_SCALE)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudScaleChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_VARIANCE)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudVarianceChanged(); });
    
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_DENSITY_X)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudDensityChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_DENSITY_Y)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudDensityChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_DENSITY_D)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudDensityChanged(); });
    
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_DETAIL_X)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudDetailChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_DETAIL_Y)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudDetailChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_DETAIL_D)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudDetailChanged(); });
    
    getChild<LLUICtrl>(FIELD_SKY_CLOUD_SCROLL_XY)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudScrollChanged(); });
    
    getChild<LLTextureCtrl>(FIELD_SKY_CLOUD_MAP)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onCloudMapChanged(); });
    getChild<LLTextureCtrl>(FIELD_SKY_CLOUD_MAP)->setDefaultImageAssetID(LLSettingsSky::GetDefaultCloudNoiseTextureId());
    getChild<LLTextureCtrl>(FIELD_SKY_CLOUD_MAP)->setAllowNoTexture(true);

    // Sun and Moon Colors
    getChild<LLUICtrl>(FIELD_SKY_SUN_COLOR)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onSunColorChanged(); });

    // Sun and Stars Settings
    getChild<LLUICtrl>(FIELD_SKY_SUN_IMAGE)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onSunImageChanged(); });
    getChild<LLTextureCtrl>(FIELD_SKY_SUN_IMAGE)->setBlankImageAssetID(LLSettingsSky::GetBlankSunTextureId());
    getChild<LLTextureCtrl>(FIELD_SKY_SUN_IMAGE)->setDefaultImageAssetID(LLSettingsSky::GetBlankSunTextureId());
    getChild<LLTextureCtrl>(FIELD_SKY_SUN_IMAGE)->setAllowNoTexture(true);
    
    getChild<LLUICtrl>(FIELD_SKY_GLOW_FOCUS)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onGlowChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_GLOW_SIZE)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onGlowChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_STAR_BRIGHTNESS)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onStarBrightnessChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_SUN_SCALE)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onSunScaleChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_SUN_AZIMUTH)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onSunAzimElevChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_SUN_ELEVATION)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onSunAzimElevChanged(); });
    
    // Moon Settings
    getChild<LLUICtrl>(FIELD_SKY_MOON_IMAGE)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onMoonImageChanged(); });
    getChild<LLTextureCtrl>(FIELD_SKY_MOON_IMAGE)->setDefaultImageAssetID(LLSettingsSky::GetDefaultMoonTextureId());
    getChild<LLTextureCtrl>(FIELD_SKY_MOON_IMAGE)->setBlankImageAssetID(LLSettingsSky::GetDefaultMoonTextureId());
    getChild<LLTextureCtrl>(FIELD_SKY_MOON_IMAGE)->setAllowNoTexture(true);
    
    getChild<LLUICtrl>(FIELD_SKY_MOON_SCALE)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onMoonScaleChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_MOON_BRIGHTNESS)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onMoonBrightnessChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_MOON_AZIMUTH)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onMoonAzimElevChanged(); });
    getChild<LLUICtrl>(FIELD_SKY_MOON_ELEVATION)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onMoonAzimElevChanged(); });


    getChild<LLColorSwatchCtrl>(FIELD_WATER_FOG_COLOR)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterFogColorChanged(); });
    getChild<LLSliderCtrl>(FIELD_WATER_FOG_DENSITY)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterFogDensityChanged(); }); // Use LLSliderCtrl
    getChild<LLSliderCtrl>(FIELD_WATER_UNDERWATER_MOD)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterUnderwaterModChanged(); }); // Use LLSliderCtrl, Renamed handler

    // Note: Texture picker for Water Normal Map has extra setup after the callback
    getChild<LLTextureCtrl>(FIELD_WATER_NORMAL_MAP)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterNormalMapChanged(); });
    LLTextureCtrl* waterNormalMapCtrl = getChild<LLTextureCtrl>(FIELD_WATER_NORMAL_MAP); // Need pointer for extra setup
    if (waterNormalMapCtrl) // Add a null check here just in case the picker is missing
    {
        waterNormalMapCtrl->setDefaultImageAssetID(LLSettingsWater::GetDefaultWaterNormalAssetId());
        waterNormalMapCtrl->setBlankImageAssetID(BLANK_OBJECT_NORMAL);
        waterNormalMapCtrl->setAllowNoTexture(true); // Allow clearing the texture
    }

    getChild<LLUICtrl>(FIELD_WATER_WAVE1_XY)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterLargeWaveChanged(); }); // LLXYVector derives from LLUICtrl, Renamed handler
    getChild<LLUICtrl>(FIELD_WATER_WAVE2_XY)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterSmallWaveChanged(); }); // LLXYVector derives from LLUICtrl, Renamed handler

    // Normal Scale uses a single handler, connect all three sliders to it
    getChild<LLSliderCtrl>(FIELD_WATER_NORMAL_SCALE_X)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterNormalScaleChanged(); }); // Use LLSliderCtrl
    getChild<LLSliderCtrl>(FIELD_WATER_NORMAL_SCALE_Y)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterNormalScaleChanged(); }); // Use LLSliderCtrl
    getChild<LLSliderCtrl>(FIELD_WATER_NORMAL_SCALE_Z)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterNormalScaleChanged(); }); // Use LLSliderCtrl

    getChild<LLSliderCtrl>(FIELD_WATER_FRESNEL_SCALE)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterFresnelScaleChanged(); }); // Use LLSliderCtrl
    getChild<LLSliderCtrl>(FIELD_WATER_FRESNEL_OFFSET)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterFresnelOffsetChanged(); }); // Use LLSliderCtrl
    getChild<LLSliderCtrl>(FIELD_WATER_SCALE_ABOVE)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterScaleAboveChanged(); }); // Use LLSliderCtrl
    getChild<LLSliderCtrl>(FIELD_WATER_SCALE_BELOW)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterScaleBelowChanged(); }); // Use LLSliderCtrl
    getChild<LLSliderCtrl>(FIELD_WATER_BLUR_MULTIP)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterBlurMultipChanged(); }); // Use LLSliderCtrl
    
    // getChild<LLUICtrl>(FIELD_SKY_SUN_ROTATION)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onSunRotationChanged(); });
    // getChild<LLUICtrl>(FIELD_SKY_MOON_ROTATION)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onMoonRotationChanged(); });
    // getChild<LLUICtrl>(BTN_RESET)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onButtonReset(); });
    // getChild<LLTextureCtrl>(FIELD_WATER_NORMAL_MAP)->setDefaultImageAssetID(LLSettingsWater::GetDefaultWaterNormalAssetId());
    // getChild<LLTextureCtrl>(FIELD_WATER_NORMAL_MAP)->setBlankImageAssetID(BLANK_OBJECT_NORMAL);
    // getChild<LLTextureCtrl>(FIELD_WATER_NORMAL_MAP)->setCommitCallback([this](LLUICtrl *, const LLSD &) { onWaterMapChanged(); });

    // >>>>>>>>>> ADDED: Initialize Camera Control UI Pointers <<<<<<<<<<
    mCamRotateStick = getChild<LLJoystickCameraRotate>("cam_rotate_stick");
    mBtnRollLeft = getChild<LLButton>("roll_left");
    mBtnRollRight = getChild<LLButton>("roll_right");
    mBtnZoomPlus = getChild<LLButton>("zoom_plus_btn");
    mBtnZoomMinus = getChild<LLButton>("zoom_minus_btn");
    mSliderZoomDistance = getChild<LLSlider>("zoom_slider"); // Get <slider_bar> as LLSlider
    mCamTrackStick = getChild<LLJoystickCameraTrack>("cam_track_stick");

    // Pointers for sliders/checkboxes linked via control_name are not needed here.

    // Initialize Mode Control Buttons & Panels
    mBtnCamPresetsView = getChild<LLButton>("presets_btn");
    mBtnCamPanView = getChild<LLButton>("pan_btn");
    mBtnCamModesView = getChild<LLButton>("avatarview_btn"); // Matches XML name

    mPanelPresetViews = getChild<LLPanel>("preset_views_list");
    mPanelCameraModes = getChild<LLPanel>("camera_modes_list");
    mPanelZoomControls = getChild<LLPanel>("zoom"); // Panel containing Orbit/Zoom/Pan

    // Optional but recommended: Check if pointers were found
    if (!mCamRotateStick || !mBtnRollLeft || !mBtnRollRight || !mBtnZoomPlus || !mBtnZoomMinus ||
        !mSliderZoomDistance || !mCamTrackStick || !mBtnCamPresetsView || !mBtnCamPanView ||
        !mBtnCamModesView || !mPanelPresetViews || !mPanelCameraModes || !mPanelZoomControls)
    {
       LL_WARNS("PhotoToolsCamera") << "postBuild: Failed to initialize one or more camera control pointers!" << LL_ENDL;
       // Depending on severity, you might return false or disable related functionality
    }
    // >>>>>>>>>> END ADDED: Initialize Camera Control UI Pointers <<<<<<<<<<

    // <AP:WW> ADD START: Initialize Luminance Weight UI Control Pointers
    mSliderLumWeightR = getChild<LLSlider>("SB_LumWeightR");
    mSpinnerLumWeightR = getChild<LLSpinCtrl>("S_LumWeightR");
    mResetLumWeightRBtn = getChild<LLButton>("ResetLumWeightRBtn");

    mSliderLumWeightG = getChild<LLSlider>("SB_LumWeightG");
    mSpinnerLumWeightG = getChild<LLSpinCtrl>("S_LumWeightG");
    mResetLumWeightGBtn = getChild<LLButton>("ResetLumWeightGBtn");

    mSliderLumWeightB = getChild<LLSlider>("SB_LumWeightB");
    mSpinnerLumWeightB = getChild<LLSpinCtrl>("S_LumWeightB");
    mResetLumWeightBBtn = getChild<LLButton>("ResetLumWeightBBtn");
    // <AP:WW> ADD END: Initialize Luminance Weight UI Control Pointers

    // <AP:WW> ADD START: Initialize Color Balance UI Control Pointers
    mSliderCB_CyanRed       = getChild<LLSlider>("SB_CB_CyanRed");
    mSpinnerCB_CyanRed      = getChild<LLSpinCtrl>("S_CB_CyanRed");
    mResetBtnCB_CyanRed     = getChild<LLButton>("Reset_CB_CyanRed");

    mSliderCB_MagentaGreen  = getChild<LLSlider>("SB_CB_MagentaGreen");
    mSpinnerCB_MagentaGreen = getChild<LLSpinCtrl>("S_CB_MagentaGreen");
    mResetBtnCB_MagentaGreen= getChild<LLButton>("Reset_CB_MagentaGreen");

    mSliderCB_YellowBlue    = getChild<LLSlider>("SB_CB_YellowBlue");
    mSpinnerCB_YellowBlue   = getChild<LLSpinCtrl>("S_CB_YellowBlue");
    mResetBtnCB_YellowBlue  = getChild<LLButton>("Reset_CB_YellowBlue");
    // <AP:WW> ADD END: Initialize Color Balance UI Control Pointers

    // <AP:WW> ADD START: Initialize UI Scale controls and set dynamic max value
    mUIScaleSlider = getChild<LLSlider>("ui_scale_slider");
    mUIScaleSpinner = getChild<LLSpinCtrl>("ui_scale_spinner");
    mResetUIScaleBtn = getChild<LLButton>("ResetUIScale");

    // <AP:WW> Port dynamic max value logic from LLFloaterPreference
    if (mUIScaleSlider && mUIScaleSpinner) // Ensure controls were found
    {
        // <AP:WW> Set DIFFERENT maximum values for slider and spinner
        mUIScaleSlider->setMaxValue(2.0f);   // Set slider maximum to 2.0
        mUIScaleSpinner->setMaxValue(3.5f);  // Set spinner maximum to 3.5

        // <AP:WW> Set DIFFERENT minimum values for slider and spinner
        mUIScaleSlider->setMinValue(0.75f);   // Set slider minimum to 0.5
        mUIScaleSpinner->setMinValue(0.25f); // Set spinner minimum to 0.25

        // <AP:WW> Sync initial value from settings
        F32 current_scale = gSavedSettings.getF32("UIScaleFactor");
        // Clamp initial value to be within the SPINNER'S range initially,
        // as it has the wider minimum range. The slider can still display this value
        // even if it's below its interactive minimum.
        current_scale = llclamp(current_scale, mUIScaleSpinner->getMinValue(), mUIScaleSpinner->getMaxValue());

        mUIScaleSlider->setValue(current_scale);
        mUIScaleSpinner->setValue(current_scale);
    }
    // <AP:WW> ADD END: Initialize UI Scale controls

    // <AP:WW> Initialize Preset Combo Box pointer
    mGraphicsPresetCombo = getChild<LLComboBox>("CB_Graphics_Preset");
    if (mGraphicsPresetCombo)
    {
        // Set initial value based on current setting
        mGraphicsPresetCombo->selectByValue(gSavedSettings.getU32("RenderQualityPerformance"));
    }

    switchViews(CAMERA_CTRL_MODE_PAN);

    refreshSettings(); 

    // <AP:WW> ADD START: Initialize Color Balance Mode and Controls
    mCurrentColorBalanceMode = gSavedSettings.getS32("APColorBalanceMode");
    refreshColorBalanceControls(); // Set initial slider/spinner values
    // <AP:WW> ADD END: Initialize Color Balance Mode and Controls


    // <AP:WW> ADD THIS BLOCK to connect the Favorites Bar checkbox
    LLCheckBoxCtrl* favBarCheckboxCtrl = getChild<LLCheckBoxCtrl>("CH_B_Show_Navbar_Favorites");
    if (favBarCheckboxCtrl) // Still good practice to check if the getChild was successful
    {
        // Set its initial state when APS opens
        favBarCheckboxCtrl->set(gSavedSettings.getBOOL("FSInternalShowNavbarFavoritesPanel")); 
        
        // Tell it what function to call when its value changes (i.e., when clicked)
        favBarCheckboxCtrl->setCommitCallback(boost::bind(&APFloaterPhototools::onAPSShowFavoritesBarToggle, this)); 
        // // <AP:WW/> Connected APS favorites bar toggle to its handler.
    }
    // </AP:WW>

    refreshSky();

    initCallbacks();

    return LLFloater::postBuild();
}

void APFloaterPhototools::onClose(bool app_quitting)
 {
     
}

// Environment Tab

void APFloaterPhototools::onShowMySettingsChanged()
{
    if (mShowMySettingsCheckbox)
    {

    }
    loadPresets(); // Refresh the preset lists
}

void APFloaterPhototools::loadPresets()
{
    LLInventoryModel::cat_array_t cats;
    LLInventoryModel::item_array_t items;
    APSettingsCollector collector;
    gInventory.collectDescendentsIf(LLUUID::null,
                                    cats,
                                    items,
                                    LLInventoryModel::EXCLUDE_TRASH,
                                    collector);

    std::multimap<std::string, LLUUID> sky_map;
    std::multimap<std::string, LLUUID> water_map;
    //  std::multimap<std::string, LLUUID> daycycle_map;

    // <AP:WW> Get checkbox state for folder filtering
    bool showMySettingsFolderOnly = false;
    if (mShowMySettingsCheckbox) // Check if the checkbox control is valid
    {
        showMySettingsFolderOnly = mShowMySettingsCheckbox->getValue();
    }

    LLUUID settingsFolderUUID = LLUUID::null;
    if (showMySettingsFolderOnly) // <AP:WW> Only find FT_SETTINGS folder if checkbox is checked
    {
        settingsFolderUUID = gInventory.findCategoryUUIDForType(LLFolderType::FT_SETTINGS);
        if (settingsFolderUUID.isNull())
        {
            LL_WARNS("PresetDebug") << "<AP:WW> WARNING: Could not find FT_SETTINGS folder in inventory!  Showing all settings." << LL_ENDL;
            showMySettingsFolderOnly = false; // Fallback to showing all if folder not found
        }
    }

    for (LLInventoryModel::item_array_t::const_iterator it = items.begin(); it != items.end(); ++it)
    {
        LLInventoryItem* item = *it;

        // <AP:WW> Conditional filtering based on "Show Only FT_Settings Folder" checkbox
        bool shouldIncludeItem = true; // By default, include

        if (showMySettingsFolderOnly)
        {
            if (!gInventory.isObjectDescendentOf(item->getUUID(), settingsFolderUUID))
            {
                shouldIncludeItem = false; // Exclude if "Show Only FT_Settings" is checked AND not in FT_SETTINGS folder
            }
        }

        if (shouldIncludeItem)
        {
            LLSettingsType::type_e type = LLSettingsType::fromInventoryFlags(item->getFlags());
            switch (type)
            {
                case LLSettingsType::ST_SKY:
                    sky_map.insert(std::make_pair(item->getName(), item->getAssetUUID()));
                    break;
                case LLSettingsType::ST_WATER:
                    water_map.insert(std::make_pair(item->getName(), item->getAssetUUID()));
                    break;
                case LLSettingsType::ST_DAYCYCLE:
                    // Intentionally ignoring Day Cycle settings in Phototools
                    break; // Add this case back, but leave it empty
                default:
                    // This will now only catch genuinely unknown/unexpected setting types
                    LL_WARNS() << "Found unhandled setting type for item: " << item->getName() << LL_ENDL;
                    break;
            }
        }
    }

    loadWaterPresets(water_map);
    loadSkyPresets(sky_map);
    // loadDayCyclePresets(daycycle_map);
}

// void APFloaterPhototools::loadDayCyclePresets(const std::multimap<std::string, LLUUID>& daycycle_map)
// {
    // mDayCyclePresetsCombo->operateOnAll(LLComboBox::OP_DELETE);
    // mDayCyclePresetsCombo->add(LLTrans::getString("QP_WL_Region_Default"), LLSD(PRESET_NAME_REGION_DEFAULT), EAddPosition::ADD_BOTTOM, false);
    // mDayCyclePresetsCombo->add(LLTrans::getString("QP_WL_None"), LLSD(PRESET_NAME_NONE), EAddPosition::ADD_BOTTOM, false);
    // mDayCyclePresetsCombo->addSeparator();

    // Add setting presets.
    // for (std::multimap<std::string, LLUUID>::const_iterator it = daycycle_map.begin(); it != daycycle_map.end(); ++it)
    // {
        // const std::string& preset_name = (*it).first;
        // const LLUUID& asset_id = (*it).second;

        // if (!preset_name.empty())
        // {
            // mDayCyclePresetsCombo->add(preset_name, LLSD(asset_id));
        // }
    // }
// <FS:Beq> Opensim legacy windlight support
// Opensim may support both environment and extenvironment caps on the same region
// we also need these disabled in SL on the OpenSim build.
// #ifdef OPENSIM
    // if(LLGridManager::getInstance()->isInOpenSim())
    // {
        // LL_DEBUGS("WindlightCaps") << "Adding legacy day cycle presets to QP" << LL_ENDL;
        // WL still supported
        // if (!daycycle_map.empty() && !LLEnvironment::getInstance()->mLegacyDayCycles.empty())
        // {
            // mDayCyclePresetsCombo->addSeparator();
        // }
        // for(const auto& preset_name : LLEnvironment::getInstance()->mLegacyDayCycles)
        // {
            // we add by name and only build the envp on demand
            // LL_DEBUGS("WindlightCaps") << "Adding legacy day cycle " << preset_name << LL_ENDL;
            // mDayCyclePresetsCombo->add(preset_name, LLSD(preset_name));
        // }
        // LL_DEBUGS("WindlightCaps") << "Done: Adding legacy day cycle presets to QP" << LL_ENDL;
    // }
// #endif
// }

void APFloaterPhototools::loadSkyPresets(const std::multimap<std::string, LLUUID>& sky_map)
{
    mWLPresetsCombo->operateOnAll(LLComboBox::OP_DELETE);
    mWLPresetsCombo->add(LLTrans::getString("QP_WL_Region_Default"), LLSD(PRESET_NAME_REGION_DEFAULT), EAddPosition::ADD_BOTTOM, false);
    mWLPresetsCombo->add(LLTrans::getString("QP_WL_Day_Cycle_Based"), LLSD(PRESET_NAME_DAY_CYCLE), EAddPosition::ADD_BOTTOM, false);
    mWLPresetsCombo->addSeparator();

    // Add setting presets.
    for (std::multimap<std::string, LLUUID>::const_iterator it = sky_map.begin(); it != sky_map.end(); ++it)
    {
        const std::string& preset_name = (*it).first;
        const LLUUID& asset_id = (*it).second;

        if (!preset_name.empty())
        {
            mWLPresetsCombo->add(preset_name, LLSD(asset_id));
        }
    }
// <FS:Beq> Opensim legacy windlight support
// Opensim may support both environment and extenvironment caps on the same region
// we also need these disabled in SL on the OpenSim build.
#ifdef OPENSIM
    if(LLGridManager::getInstance()->isInOpenSim())
    {
        LL_DEBUGS("WindlightCaps") << "Adding legacy sky presets to QP" << LL_ENDL;
        // WL still supported
        if (!sky_map.empty() && !LLEnvironment::getInstance()->mLegacySkies.empty())
        {
            mWLPresetsCombo->addSeparator();
        }
        for(const auto& preset_name : LLEnvironment::getInstance()->mLegacySkies)
        {
            // we add by name and only build the envp on demand
            LL_DEBUGS("WindlightCaps") << "Adding legacy sky " << preset_name << LL_ENDL;
            // append "WL" to denote legacy. Have to create a new string not update the reference.
            mWLPresetsCombo->add(preset_name+ "[WL]", LLSD(preset_name));
        }
        LL_DEBUGS("WindlightCaps") << "Done: Adding legacy sky presets to QP" << LL_ENDL;
    }
#endif
}

void APFloaterPhototools::loadWaterPresets(const std::multimap<std::string, LLUUID>& water_map)
{
    mWaterPresetsCombo->operateOnAll(LLComboBox::OP_DELETE);
    mWaterPresetsCombo->add(LLTrans::getString("QP_WL_Region_Default"), LLSD(PRESET_NAME_REGION_DEFAULT), EAddPosition::ADD_BOTTOM, false);
    mWaterPresetsCombo->add(LLTrans::getString("QP_WL_Day_Cycle_Based"), LLSD(PRESET_NAME_DAY_CYCLE), EAddPosition::ADD_BOTTOM, false);
    mWaterPresetsCombo->addSeparator();

    // Add setting presets.
    for (std::multimap<std::string, LLUUID>::const_iterator it = water_map.begin(); it != water_map.end(); ++it)
    {
        const std::string& preset_name = (*it).first;
        const LLUUID& asset_id = (*it).second;

        if (!preset_name.empty())
        {
            mWaterPresetsCombo->add(preset_name, LLSD(asset_id));
        }
    }
// <FS:Beq> Opensim legacy windlight support
// Opensim may support both environment and extenvironment caps on the same region
// we also need these disabled in SL on the OpenSim build.
#ifdef OPENSIM
    if(LLGridManager::getInstance()->isInOpenSim())
    {
        LL_DEBUGS("WindlightCaps") << "Adding legacy presets to QP" << LL_ENDL;
        // WL still supported
        if (!water_map.empty() && !LLEnvironment::getInstance()->mLegacyWater.empty())
        {
            mWaterPresetsCombo->addSeparator();
        }
        for(const auto& preset_name : LLEnvironment::getInstance()->mLegacyWater)
        {
            // we add by name and only build the envp on demand
            LL_DEBUGS("WindlightCaps") << "Adding legacy water " << preset_name << LL_ENDL;
            mWaterPresetsCombo->add(preset_name, LLSD(preset_name));
        }
        LL_DEBUGS("WindlightCaps") << "Done: Adding legacy water presets to QP" << LL_ENDL;
    }
#endif
}

void APFloaterPhototools::setDefaultPresetsEnabled(bool enabled)
{
    LLScrollListItem* item{ nullptr };

    item = mWLPresetsCombo->getItemByValue(LLSD(PRESET_NAME_REGION_DEFAULT));
    if (item) item->setEnabled(enabled);

    item = mWLPresetsCombo->getItemByValue(LLSD(PRESET_NAME_DAY_CYCLE));
    if (item) item->setEnabled(enabled);

    item = mWaterPresetsCombo->getItemByValue(LLSD(PRESET_NAME_REGION_DEFAULT));
    if (item) item->setEnabled(enabled);

    item = mWaterPresetsCombo->getItemByValue(LLSD(PRESET_NAME_DAY_CYCLE));
    if (item) item->setEnabled(enabled);

    // item = mDayCyclePresetsCombo->getItemByValue(LLSD(PRESET_NAME_REGION_DEFAULT));
    // if (item) item->setEnabled(enabled);

    // item = mDayCyclePresetsCombo->getItemByValue(LLSD(PRESET_NAME_NONE));
    // if (item) item->setEnabled(enabled);
}

void APFloaterPhototools::setSelectedEnvironment()
{
    //LL_INFOS() << "EEP: getSelectedEnvironment: " << LLEnvironment::instance().getSelectedEnvironment() << LL_ENDL;

    setDefaultPresetsEnabled(true);
    mWLPresetsCombo->selectByValue(LLSD(PRESET_NAME_REGION_DEFAULT));
    mWaterPresetsCombo->selectByValue(LLSD(PRESET_NAME_REGION_DEFAULT));
    // mDayCyclePresetsCombo->selectByValue(LLSD(PRESET_NAME_REGION_DEFAULT));

    if (LLEnvironment::instance().getSelectedEnvironment() == LLEnvironment::ENV_LOCAL)
    {
        // Day cycle, fixed sky and fixed water may all be set at the same time
        // Check and set day cycle first. Fixed sky and water both override
        // the sky and water settings in a day cycle, so check them after the
        // day cycle. If no fixed sky or fixed water is set, they are either
        // defined in the day cycle or inherited from a higher environment level.
        LLSettingsDay::ptr_t day = LLEnvironment::instance().getEnvironmentDay(LLEnvironment::ENV_LOCAL);
        if (day)
        {
            //LL_INFOS() << "EEP: day name = " << day->getName() << " - asset id = " << day->getAssetId() << LL_ENDL;
            if( day->getAssetId().notNull())
            { // EEP processing
                // mDayCyclePresetsCombo->selectByValue(LLSD(day->getAssetId()));
                // Sky and Water are part of a day cycle in EEP
                mWLPresetsCombo->selectByValue(LLSD(PRESET_NAME_DAY_CYCLE));
                mWaterPresetsCombo->selectByValue(LLSD(PRESET_NAME_DAY_CYCLE));
            }
#ifdef OPENSIM
            else if (LLGridManager::getInstance()->isInOpenSim())
            {
                auto preset_name = day->getName();
                LL_DEBUGS("WindlightCaps") << "Current Day cycle is " << preset_name << LL_ENDL;
                if (preset_name == "_default_")
                {
                    preset_name = "Default";
                }
                // mDayCyclePresetsCombo->selectByValue(preset_name);
                // Sky is part of day so treat that as day cycle
                mWLPresetsCombo->selectByValue(LLSD(PRESET_NAME_DAY_CYCLE));
                // Water is not part of legacy day so we need to hunt around
                LLSettingsWater::ptr_t water = LLEnvironment::instance().getEnvironmentFixedWater(LLEnvironment::ENV_LOCAL);
                if (water)
                {
                    // This is going to be possible. OS will support both Legacy and EEP
                    // so having a water EEP asset with a Legacy day cycle could happen.
                    LLUUID asset_id = water->getAssetId();
                    if (asset_id.notNull())
                    {
                        mWaterPresetsCombo->selectByValue(LLSD(asset_id));
                    }
                    else
                    {
                        std::string preset_name = water->getName();
                        if (preset_name == "_default_")
                        {
                            preset_name = "Default";
                        }
                        mWaterPresetsCombo->selectByValue(preset_name);
                    }
                }
            }
#endif //OPENSIM
        }
        else
        {
            // mDayCyclePresetsCombo->selectByValue(LLSD(PRESET_NAME_NONE));
        }

        LLSettingsSky::ptr_t sky = LLEnvironment::instance().getEnvironmentFixedSky(LLEnvironment::ENV_LOCAL);
        if (sky)
        {
            //LL_INFOS() << "EEP: sky name = " << sky->getName() << " - asset id = " << sky->getAssetId() << LL_ENDL;
            if(sky->getAssetId().notNull())
            {
                mWLPresetsCombo->selectByValue(LLSD(sky->getAssetId()));
            }
#ifdef OPENSIM
            else if (LLGridManager::getInstance()->isInOpenSim())
            {
                auto preset_name = sky->getName();
                LL_DEBUGS("WindlightCaps") << "Current Sky is " << preset_name << LL_ENDL;
                if (preset_name == "_default_")
                {
                    preset_name = "Default";
                }
                mWLPresetsCombo->selectByValue(preset_name);
            }
#endif
        }
        // Water is not part of legacy day so we need to hunt around
        LLSettingsWater::ptr_t water = LLEnvironment::instance().getEnvironmentFixedWater(LLEnvironment::ENV_LOCAL);
        if (water)
        {
            LLUUID asset_id = water->getAssetId();
            if (asset_id.notNull())
            {
                mWaterPresetsCombo->selectByValue(LLSD(asset_id));
            }
#ifdef OPENSIM
            else if (LLGridManager::getInstance()->isInOpenSim())
            {
                auto preset_name = water->getName();
                if (preset_name == "_default_")
                {
                    preset_name = "Default";
                }
                mWaterPresetsCombo->selectByValue(preset_name);
            }
#endif //OPENSIM
        }
    }
    else
    {
        // LLEnvironment::ENV_REGION:
        // LLEnvironment::ENV_PARCEL:
        mWLPresetsCombo->selectByValue(LLSD(PRESET_NAME_REGION_DEFAULT));
        // mWaterPresetsCombo->selectByValue(LLSD(PRESET_NAME_REGION_DEFAULT));
        // mDayCyclePresetsCombo->selectByValue(LLSD(PRESET_NAME_REGION_DEFAULT));
    }

    setDefaultPresetsEnabled(false);
}

bool APFloaterPhototools::isValidPreset(const LLSD& preset)
{
    if (preset.isUUID())
    {
        if(!preset.asUUID().isNull()){ return true;}
    }
    else if (preset.isString())
    {
        if(!preset.asString().empty() &&
            preset.asString() != PRESET_NAME_REGION_DEFAULT &&
            preset.asString() != PRESET_NAME_DAY_CYCLE &&
            preset.asString() != PRESET_NAME_NONE)
        {
            return true;
        }
    }
    return false;
}

void APFloaterPhototools::stepComboBox(LLComboBox* ctrl, bool forward)
{
    S32 increment = (forward ? 1 : -1);
    S32 lastitem = ctrl->getItemCount() - 1;
    S32 curid = ctrl->getCurrentIndex();
    S32 startid = curid;

    do
    {
        curid += increment;
        if (curid < 0)
        {
            curid = lastitem;
        }
        else if (curid > lastitem)
        {
            curid = 0;
        }
        ctrl->setCurrentByIndex(curid);
    }
    while (!isValidPreset(ctrl->getSelectedValue()) && curid != startid);
}

void APFloaterPhototools::selectSkyPreset(const LLSD& preset)
{
// Opensim continued W/L support
#ifdef OPENSIM
    if(!preset.isUUID() && LLGridManager::getInstance()->isInOpenSim())
    {
        LLSettingsSky::ptr_t legacy_sky = nullptr;
        LLSD messages;

        legacy_sky = LLEnvironment::createSkyFromLegacyPreset(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "windlight", "skies", preset.asString() + ".xml"), messages);

        if (legacy_sky)
        {
            // Need to preserve current sky manually in this case in contrast to asset-based settings
            LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
            LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, legacy_sky, current_water);
            LLEnvironment::instance().setSelectedEnvironment(LLEnvironment::ENV_LOCAL);
            LLEnvironment::instance().updateEnvironment(static_cast<LLSettingsBase::Seconds>(gSavedSettings.getF32("FSEnvironmentManualTransitionTime")));
        }
        else
        {
            LL_WARNS() << "Legacy windlight conversion failed for " << preset << " existing env unchanged." << LL_ENDL;
            return;
        }
    }
    else // note the else here bridges the endif
#endif
    {
        LLEnvironment::instance().setSelectedEnvironment(LLEnvironment::ENV_LOCAL);
        LLEnvironment::instance().setManualEnvironment(LLEnvironment::ENV_LOCAL, preset.asUUID());
    }
}

void APFloaterPhototools::selectWaterPreset(const LLSD& preset)
{
#ifdef OPENSIM
    if(!preset.isUUID() && LLGridManager::getInstance()->isInOpenSim())
    {
        LLSettingsWater::ptr_t legacy_water = nullptr;
        LLSD messages;
        legacy_water = LLEnvironment::createWaterFromLegacyPreset(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "windlight", "water", preset.asString() + ".xml"), messages);
        if (legacy_water)
        {
            // Need to preserve current sky manually in this case in contrast to asset-based settings
            LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
            LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, legacy_water);
            LLEnvironment::instance().setSelectedEnvironment(LLEnvironment::ENV_LOCAL);
            LLEnvironment::instance().updateEnvironment(static_cast<LLSettingsBase::Seconds>(gSavedSettings.getF32("FSEnvironmentManualTransitionTime")));
        }
        else
        {
            LL_WARNS() << "Legacy windlight conversion failed for " << preset << " existing env unchanged." << LL_ENDL;
            return;
        }
    }
    else // beware the trailing else here.
#endif
    {
        LLEnvironment::instance().setSelectedEnvironment(LLEnvironment::ENV_LOCAL);
        LLEnvironment::instance().setManualEnvironment(LLEnvironment::ENV_LOCAL, preset.asUUID());
    }
}

// void APFloaterPhototools::selectDayCyclePreset(const LLSD& preset)
// {
// #ifdef OPENSIM
    // if(!preset.isUUID() && LLGridManager::getInstance()->isInOpenSim())
    // {
        // LLSettingsDay::ptr_t legacyday = nullptr;
        // LLSD messages;
        // legacyday = LLEnvironment::createDayCycleFromLegacyPreset(gDirUtilp->getExpandedFilename(LL_PATH_APP_SETTINGS, "windlight", "days", preset.asString() + ".xml"), messages);
        // if (legacyday)
        // {
            // LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, legacyday);
            // LLEnvironment::instance().setSelectedEnvironment(LLEnvironment::ENV_LOCAL);
            // LLEnvironment::instance().updateEnvironment(static_cast<LLSettingsBase::Seconds>(gSavedSettings.getF32("FSEnvironmentManualTransitionTime")));
        // }
        // else
        // {
            // LL_WARNS() << "Legacy windlight conversion failed for " << preset << " existing env unchanged." << LL_ENDL;
            // return;
        // }
    // }
    // else // beware trailing else that bridges the endif
// #endif
    // {
        // LLEnvironment::instance().setSelectedEnvironment(LLEnvironment::ENV_LOCAL);
        // LLEnvironment::instance().setManualEnvironment(LLEnvironment::ENV_LOCAL, preset.asUUID());
    // }
// }

void APFloaterPhototools::onChangeWaterPreset()
{
    if (!isValidPreset(mWaterPresetsCombo->getSelectedValue()))
    {
        stepComboBox(mWaterPresetsCombo, true);
    }

    if (isValidPreset(mWaterPresetsCombo->getSelectedValue()))
    {
        selectWaterPreset(mWaterPresetsCombo->getSelectedValue());
    }
    else
    {
        LLNotificationsUtil::add("NoValidEnvSettingFound");
    }
}

void APFloaterPhototools::onChangeSkyPreset()
{
    if (!isValidPreset(mWLPresetsCombo->getSelectedValue()))
    {
        stepComboBox(mWLPresetsCombo, true);
    }

    if (isValidPreset(mWLPresetsCombo->getSelectedValue()))
    {
        selectSkyPreset(mWLPresetsCombo->getSelectedValue());
    }
    else
    {
        LLNotificationsUtil::add("NoValidEnvSettingFound");
    }
}

// void APFloaterPhototools::onChangeDayCyclePreset()
// {
    // if (!isValidPreset(mDayCyclePresetsCombo->getSelectedValue()))
    // {
        // stepComboBox(mDayCyclePresetsCombo, true);
    // }

    // if (isValidPreset(mDayCyclePresetsCombo->getSelectedValue()))
    // {
        // selectDayCyclePreset(mDayCyclePresetsCombo->getSelectedValue());
    // }
    // else
    // {
        // LLNotificationsUtil::add("NoValidEnvSettingFound");
    // }
// }

void APFloaterPhototools::onClickWaterPrev()
{
    stepComboBox(mWaterPresetsCombo, false);
    selectWaterPreset(mWaterPresetsCombo->getSelectedValue());
}

void APFloaterPhototools::onClickWaterNext()
{
    stepComboBox(mWaterPresetsCombo, true);
    selectWaterPreset(mWaterPresetsCombo->getSelectedValue());
}

void APFloaterPhototools::onClickSkyPrev()
{
    stepComboBox(mWLPresetsCombo, false);
    selectSkyPreset(mWLPresetsCombo->getSelectedValue());
}

void APFloaterPhototools::onClickSkyNext()
{
    stepComboBox(mWLPresetsCombo, true);
    selectSkyPreset(mWLPresetsCombo->getSelectedValue());
}

// void APFloaterPhototools::onClickDayCyclePrev()
// {
    // stepComboBox(mDayCyclePresetsCombo, false);
    // selectDayCyclePreset(mDayCyclePresetsCombo->getSelectedValue());
// }

// void APFloaterPhototools::onClickDayCycleNext()
// {
    // stepComboBox(mDayCyclePresetsCombo, true);
    // selectDayCyclePreset(mDayCyclePresetsCombo->getSelectedValue());
// }

void APFloaterPhototools::setSelectedSky(const std::string& preset_name)
{
    mWLPresetsCombo->setValue(LLSD(preset_name));
}

void APFloaterPhototools::setSelectedWater(const std::string& preset_name)
{
    mWaterPresetsCombo->setValue(LLSD(preset_name));
}

// void APFloaterPhototools::setSelectedDayCycle(const std::string& preset_name)
// {
    // mDayCyclePresetsCombo->setValue(LLSD(preset_name));
    // mWLPresetsCombo->setValue(LLSD(PRESET_NAME_DAY_CYCLE));
    // mWaterPresetsCombo->setValue(LLSD(PRESET_NAME_DAY_CYCLE));
// }

// Shadows Tab //

void APFloaterPhototools::refreshSettings()
{

    LLPanel::refresh();

    LLVector3 renderShadowSplitExponent = gSavedSettings.getVector3("RenderShadowSplitExponent");

    mSliderRenderShadowSplitExponentY->setValue(renderShadowSplitExponent.mV[VY]);
    mSpinnerRenderShadowSplitExponentY->setValue(renderShadowSplitExponent.mV[VY]);

    LLVector3 renderRenderShadowGaussian = gSavedSettings.getVector3("RenderShadowGaussian");
     
    mSliderRenderShadowGaussianX->setValue(renderRenderShadowGaussian.mV[VX]);
    mSpinnerRenderShadowGaussianX->setValue(renderRenderShadowGaussian.mV[VX]);

    mSliderRenderShadowGaussianY->setValue(renderRenderShadowGaussian.mV[VY]);
    mSpinnerRenderShadowGaussianY->setValue(renderRenderShadowGaussian.mV[VY]);
    
    LLVector3 renderSSAOEffect = gSavedSettings.getVector3("RenderSSAOEffect");
    
    mSpinnerRenderSSAOEffectX->setValue(renderSSAOEffect.mV[VX]);
    mSliderRenderSSAOEffectX->setValue(renderSSAOEffect.mV[VX]);

    mSpinnerRenderSSAOEffectY->setValue(renderSSAOEffect.mV[VY]);
    mSliderRenderSSAOEffectY->setValue(renderSSAOEffect.mV[VY]);

    // <AP:WW> CORRECTED START: Update Luminance Weight controls within refreshSettings (No IF checks)
    LLVector3 currentWeights = gSavedSettings.getVector3("APLuminanceWeights");

    // Directly use the pointers obtained in postBuild
    mSliderLumWeightR->setValue(currentWeights.mV[0]);
    mSpinnerLumWeightR->setValue(currentWeights.mV[0]);

    mSliderLumWeightG->setValue(currentWeights.mV[1]);
    mSpinnerLumWeightG->setValue(currentWeights.mV[1]);

    mSliderLumWeightB->setValue(currentWeights.mV[2]);
    mSpinnerLumWeightB->setValue(currentWeights.mV[2]);
    // <AP:WW> CORRECTED END: Update Luminance Weight controls

    refreshCameraControls(); 
}

void APFloaterPhototools::onChangeRenderShadowSplitExponentSliderY()
{
    LLVector3 renderShadowSplitExponent = gSavedSettings.getVector3("RenderShadowSplitExponent");
    renderShadowSplitExponent.mV[VY] = mSliderRenderShadowSplitExponentY->getValueF32();
    mSpinnerRenderShadowSplitExponentY->setValue(renderShadowSplitExponent.mV[VY]);
    gSavedSettings.setVector3("RenderShadowSplitExponent", renderShadowSplitExponent);
}

void APFloaterPhototools::onChangeRenderShadowSplitExponentSpinnerY()
{
    LLVector3 renderShadowSplitExponent = gSavedSettings.getVector3("RenderShadowSplitExponent");
    renderShadowSplitExponent.mV[VY] = mSpinnerRenderShadowSplitExponentY->getValueF32();
    mSliderRenderShadowSplitExponentY->setValue(renderShadowSplitExponent.mV[VY]);
    gSavedSettings.setVector3("RenderShadowSplitExponent", renderShadowSplitExponent);
}

void APFloaterPhototools::onClickResetRenderShadowSplitExponentY()
{
    LLVector3 renderShadowSplitExponentDefault = LLVector3(gSavedSettings.getControl("RenderShadowSplitExponent")->getDefault());
    LLVector3 renderShadowSplitExponent = gSavedSettings.getVector3("RenderShadowSplitExponent");
    renderShadowSplitExponent.mV[VY] = renderShadowSplitExponentDefault.mV[VY];
    mSpinnerRenderShadowSplitExponentY->setValue(renderShadowSplitExponent.mV[VY]);
    mSliderRenderShadowSplitExponentY->setValue(renderShadowSplitExponent.mV[VY]);
    gSavedSettings.setVector3("RenderShadowSplitExponent", renderShadowSplitExponent);
}

void APFloaterPhototools::onChangeRenderShadowGaussianSlider()
{
    LLVector3 renderShadowGaussian = gSavedSettings.getVector3("RenderShadowGaussian");
    renderShadowGaussian.mV[VX] = mSliderRenderShadowGaussianX->getValueF32();
    renderShadowGaussian.mV[VY] = mSliderRenderShadowGaussianY->getValueF32();
    mSpinnerRenderShadowGaussianX->setValue(renderShadowGaussian.mV[VX]);
    mSpinnerRenderShadowGaussianY->setValue(renderShadowGaussian.mV[VY]);
    gSavedSettings.setVector3("RenderShadowGaussian", renderShadowGaussian);
}

void APFloaterPhototools::onChangeRenderShadowGaussianSpinner()
{
    LLVector3 renderShadowGaussian = gSavedSettings.getVector3("RenderShadowGaussian");
    renderShadowGaussian.mV[VX] = mSpinnerRenderShadowGaussianX->getValueF32();
    renderShadowGaussian.mV[VY] = mSpinnerRenderShadowGaussianY->getValueF32();
    mSliderRenderShadowGaussianX->setValue(renderShadowGaussian.mV[VX]);
    mSliderRenderShadowGaussianY->setValue(renderShadowGaussian.mV[VY]);
    gSavedSettings.setVector3("RenderShadowGaussian", renderShadowGaussian);
}

void APFloaterPhototools::onClickResetRenderShadowGaussianX()
{
    LLVector3 renderShadowGaussianDefault = LLVector3(gSavedSettings.getControl("RenderShadowGaussian")->getDefault());
    LLVector3 renderShadowGaussian = gSavedSettings.getVector3("RenderShadowGaussian");
    renderShadowGaussian.mV[VX] = renderShadowGaussianDefault.mV[VX];
    mSpinnerRenderShadowGaussianX->setValue(renderShadowGaussian.mV[VX]);
    mSliderRenderShadowGaussianX->setValue(renderShadowGaussian.mV[VX]);
    gSavedSettings.setVector3("RenderShadowGaussian", renderShadowGaussian);
}

void APFloaterPhototools::onClickResetRenderShadowGaussianY()
{
    LLVector3 renderShadowGaussianDefault = LLVector3(gSavedSettings.getControl("RenderShadowGaussian")->getDefault());
    LLVector3 renderShadowGaussian = gSavedSettings.getVector3("RenderShadowGaussian");
    renderShadowGaussian.mV[VY] = renderShadowGaussianDefault.mV[VY];
    mSpinnerRenderShadowGaussianY->setValue(renderShadowGaussian.mV[VY]);
    mSliderRenderShadowGaussianY->setValue(renderShadowGaussian.mV[VY]);
    gSavedSettings.setVector3("RenderShadowGaussian", renderShadowGaussian);
}

void APFloaterPhototools::onChangeRenderSSAOEffectSlider()
{
    LLVector3 renderSSAOEffect = gSavedSettings.getVector3("RenderSSAOEffect");
    renderSSAOEffect.mV[VX] = mSliderRenderSSAOEffectX->getValueF32();
    mSpinnerRenderSSAOEffectX->setValue(renderSSAOEffect.mV[VX]);
    gSavedSettings.setVector3("RenderSSAOEffect", renderSSAOEffect);
}

void APFloaterPhototools::onChangeRenderSSAOEffectSpinner()
{
    LLVector3 renderSSAOEffect = gSavedSettings.getVector3("RenderSSAOEffect");
    renderSSAOEffect.mV[VX] = mSpinnerRenderSSAOEffectX->getValueF32();
    mSliderRenderSSAOEffectX->setValue(renderSSAOEffect.mV[VX]);
    gSavedSettings.setVector3("RenderSSAOEffect", renderSSAOEffect);
}

void APFloaterPhototools::onClickResetRenderSSAOEffectX()
{
    LLVector3 renderSSAOEffectDefault = LLVector3(gSavedSettings.getControl("RenderSSAOEffect")->getDefault());
    LLVector3 renderSSAOEffect = gSavedSettings.getVector3("RenderSSAOEffect");
    renderSSAOEffect.mV[VX] = renderSSAOEffectDefault.mV[VX];
    mSpinnerRenderSSAOEffectX->setValue(renderSSAOEffect.mV[VX]);
    mSliderRenderSSAOEffectX->setValue(renderSSAOEffect.mV[VX]);
    gSavedSettings.setVector3("RenderSSAOEffect", renderSSAOEffect);
}

void APFloaterPhototools::onChangeRenderSSAOEffectSliderY()
{
    LLVector3 renderSSAOEffect = gSavedSettings.getVector3("RenderSSAOEffect");
    renderSSAOEffect.mV[VY] = mSliderRenderSSAOEffectY->getValueF32();
    mSpinnerRenderSSAOEffectY->setValue(renderSSAOEffect.mV[VY]);
    gSavedSettings.setVector3("RenderSSAOEffect", renderSSAOEffect);
}

void APFloaterPhototools::onChangeRenderSSAOEffectSpinnerY()
{
    LLVector3 renderSSAOEffect = gSavedSettings.getVector3("RenderSSAOEffect");
    renderSSAOEffect.mV[VY] = mSpinnerRenderSSAOEffectY->getValueF32();
    mSliderRenderSSAOEffectY->setValue(renderSSAOEffect.mV[VY]);
    gSavedSettings.setVector3("RenderSSAOEffect", renderSSAOEffect);
}

void APFloaterPhototools::onClickResetRenderSSAOEffectY()
{
    LLVector3 renderSSAOEffectDefault = LLVector3(gSavedSettings.getControl("RenderSSAOEffect")->getDefault());
    LLVector3 renderSSAOEffect = gSavedSettings.getVector3("RenderSSAOEffect");
    renderSSAOEffect.mV[VY] = renderSSAOEffectDefault.mV[VY];
    mSpinnerRenderSSAOEffectY->setValue(renderSSAOEffect.mV[VY]);
    mSliderRenderSSAOEffectY->setValue(renderSSAOEffect.mV[VY]);
    gSavedSettings.setVector3("RenderSSAOEffect", renderSSAOEffect);
}

void APFloaterPhototools::updateMaxNonImpostors(const LLSD& newvalue)
{
    // Called when the IndirectMaxNonImpostors control changes
    // Responsible for fixing the setting RenderAvatarMaxNonImpostors
    U32 value = newvalue.asInteger();

    if (0 == value || LLVOAvatar::NON_IMPOSTORS_MAX_SLIDER <= value)
    {
        value=0;
    }
    gSavedSettings.setU32("RenderAvatarMaxNonImpostors", value);
    LLVOAvatar::updateImpostorRendering(value); // make it effective immediately
}

void APFloaterPhototools::updateMaxComplexity()
{
    // Called when the IndirectMaxComplexity control changes
    LLAvatarComplexityControls::updateMax(mMaxComplexitySlider, mMaxComplexityLabel);
}

// void APFloaterPhototools::updateMaxComplexityLabel(const LLSD& newvalue)
// {
    // U32 value = newvalue.asInteger();

    // LLAvatarComplexityControls::setText(value, mMaxComplexityLabel);
// }

void APFloaterPhototools::refreshAvatarComplexityControlsFromSetting(const LLSD& new_setting_value_unused)
{

    if (mMaxComplexitySlider && mMaxComplexityLabel) 
    {
        LLAvatarComplexityControls::setIndirectMaxArc();
        U32 actual_max_arc = gSavedSettings.getU32("RenderAvatarMaxComplexity");
        LLAvatarComplexityControls::setText(actual_max_arc, mMaxComplexityLabel);
    }
}

void APFloaterPhototools::onAmbientLightChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setAmbientColor(LLColor3(getChild<LLColorSwatchCtrl>(FIELD_SKY_AMBIENT_LIGHT)->get() * SLIDER_SCALE_SUN_AMBIENT));
    sky->update();
}

void APFloaterPhototools::onBlueHorizonChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setBlueHorizon(LLColor3(getChild<LLColorSwatchCtrl>(FIELD_SKY_BLUE_HORIZON)->get() * SLIDER_SCALE_BLUE_HORIZON_DENSITY));
    sky->update();
}

void APFloaterPhototools::onBlueDensityChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setBlueDensity(LLColor3(getChild<LLColorSwatchCtrl>(FIELD_SKY_BLUE_DENSITY)->get() * SLIDER_SCALE_BLUE_HORIZON_DENSITY));
    sky->update();
}

void APFloaterPhototools::onHazeHorizonChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setHazeHorizon((F32)getChild<LLUICtrl>(FIELD_SKY_HAZE_HORIZON)->getValue().asReal());
    sky->update();
}

void APFloaterPhototools::onHazeDensityChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setHazeDensity((F32)getChild<LLUICtrl>(FIELD_SKY_HAZE_DENSITY)->getValue().asReal());
    sky->update();
}

void APFloaterPhototools::onDensityMultipChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    F32 density_mult = (F32)getChild<LLUICtrl>(FIELD_SKY_DENSITY_MULTIP)->getValue().asReal();
    density_mult *= SLIDER_SCALE_DENSITY_MULTIPLIER;
    sky->setDensityMultiplier(density_mult);
    sky->update();

}

void APFloaterPhototools::onDistanceMultipChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setDistanceMultiplier((F32)getChild<LLUICtrl>(FIELD_SKY_DISTANCE_MULTIP)->getValue().asReal());
    sky->update();

}

void APFloaterPhototools::onMaxAltChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setMaxY((F32)getChild<LLUICtrl>(FIELD_SKY_MAX_ALT)->getValue().asReal());
    sky->update();

}

void APFloaterPhototools::onSceneGammaChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setGamma((F32)getChild<LLUICtrl>(FIELD_SKY_SCENE_GAMMA)->getValue().asReal());
    sky->update();
}

void APFloaterPhototools::onReflectionProbeAmbianceChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    F32 ambiance = (F32)getChild<LLUICtrl>(FIELD_REFLECTION_PROBE_AMBIANCE)->getValue().asReal();
    sky->setReflectionProbeAmbiance(ambiance);

    // updateGammaLabel(sky);
    sky->update();
}

void APFloaterPhototools::updateGammaLabel(LLSettingsSky::ptr_t sky) 
{
    if (!sky)
        return;
    static LLCachedControl<bool> should_auto_adjust(gSavedSettings, "RenderSkyAutoAdjustLegacy", false);
    F32 ambiance = sky->getReflectionProbeAmbiance(should_auto_adjust);
    if (ambiance != 0.f)
    {
        childSetValue("scene_gamma_label", LLSD(getString("hdr_string")));
        getChild<LLUICtrl>(FIELD_SKY_SCENE_GAMMA)->setToolTip(getString("hdr_tooltip"));
    }
    else
    {
        childSetValue("scene_gamma_label", LLSD(getString("brightness_string")));
        getChild<LLUICtrl>(FIELD_SKY_SCENE_GAMMA)->setToolTip(std::string());
    }
}

void APFloaterPhototools::onMoistureLevelChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    F32 moisture_level = (F32)getChild<LLUICtrl>(FIELD_SKY_DENSITY_MOISTURE_LEVEL)->getValue().asReal();
    sky->setSkyMoistureLevel(moisture_level);
    sky->update();

}

void APFloaterPhototools::onDropletRadiusChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    F32 droplet_radius = (F32)getChild<LLUICtrl>(FIELD_SKY_DENSITY_DROPLET_RADIUS)->getValue().asReal();
    sky->setSkyDropletRadius(droplet_radius);
    sky->update();

}

void APFloaterPhototools::onIceLevelChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    F32 ice_level = (F32)getChild<LLUICtrl>(FIELD_SKY_DENSITY_ICE_LEVEL)->getValue().asReal();
    sky->setSkyIceLevel(ice_level);
    sky->update();

}

// Clouds

void APFloaterPhototools::onCloudColorChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setCloudColor(LLColor3(getChild<LLColorSwatchCtrl>(FIELD_SKY_CLOUD_COLOR)->get()));
    sky->update();
}

void APFloaterPhototools::onCloudCoverageChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setCloudShadow((F32)getChild<LLUICtrl>(FIELD_SKY_CLOUD_COVERAGE)->getValue().asReal());
    sky->update();
}

void APFloaterPhototools::onCloudScaleChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setCloudScale((F32)getChild<LLUICtrl>(FIELD_SKY_CLOUD_SCALE)->getValue().asReal());
    sky->update();
}

void APFloaterPhototools::onCloudVarianceChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setCloudVariance((F32)getChild<LLUICtrl>(FIELD_SKY_CLOUD_VARIANCE)->getValue().asReal());
}

void APFloaterPhototools::onCloudDensityChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    LLColor3 density((F32)getChild<LLUICtrl>(FIELD_SKY_CLOUD_DENSITY_X)->getValue().asReal(),
        (F32)getChild<LLUICtrl>(FIELD_SKY_CLOUD_DENSITY_Y)->getValue().asReal(),
        (F32)getChild<LLUICtrl>(FIELD_SKY_CLOUD_DENSITY_D)->getValue().asReal());

    sky->setCloudPosDensity1(density);
}

void APFloaterPhototools::onCloudDetailChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    LLColor3 detail((F32)getChild<LLUICtrl>(FIELD_SKY_CLOUD_DETAIL_X)->getValue().asReal(),
        (F32)getChild<LLUICtrl>(FIELD_SKY_CLOUD_DETAIL_Y)->getValue().asReal(),
        (F32)getChild<LLUICtrl>(FIELD_SKY_CLOUD_DETAIL_D)->getValue().asReal());

    sky->setCloudPosDensity2(detail);
}

void APFloaterPhototools::onCloudScrollChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    LLVector2 scroll(getChild<LLUICtrl>(FIELD_SKY_CLOUD_SCROLL_XY)->getValue());
    sky->setCloudScrollRate(scroll);
}

// void APFloaterPhototools::onCloudMapChanged()
// {
    // LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    // if (!sky)
        // return;
    // LLTextureCtrl* ctrl = getChild<LLTextureCtrl>(FIELD_SKY_CLOUD_MAP);
    // sky->setCloudNoiseTextureId(ctrl->getValue().asUUID());
    // sky->update(); 
// }

void APFloaterPhototools::onCloudMapChanged()
{
    LLEnvironment::instance().setSelectedEnvironment(LLEnvironment::ENV_LOCAL);

    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_sky)
    {
        return;
    }

    LLTextureCtrl* picker_ctrl = getChild<LLTextureCtrl>(FIELD_SKY_CLOUD_MAP);
    if (!picker_ctrl)
    {
         return;
    }

    LLUUID new_texture_id = picker_ctrl->getValue().asUUID();
    LLSettingsSky::ptr_t sky_to_set = current_sky->buildClone();
    sky_to_set->setCloudNoiseTextureId(new_texture_id);

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, sky_to_set);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
    picker_ctrl->setValue(new_texture_id);
}

void APFloaterPhototools::onSunColorChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    LLColor3 color(getChild<LLColorSwatchCtrl>(FIELD_SKY_SUN_COLOR)->get());

    color *= SLIDER_SCALE_SUN_AMBIENT;

    sky->setSunlightColor(color);
    sky->update();
}

void APFloaterPhototools::onSunImageChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setSunTextureId(getChild<LLTextureCtrl>(FIELD_SKY_SUN_IMAGE)->getValue().asUUID());
    sky->update();
}

void APFloaterPhototools::onGlowChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    LLColor3 glow((F32)getChild<LLUICtrl>(FIELD_SKY_GLOW_SIZE)->getValue().asReal(), 0.0f, (F32)getChild<LLUICtrl>(FIELD_SKY_GLOW_FOCUS)->getValue().asReal());

    // takes 0 - 1.99 UI range -> 40 -> 0.2 range
    glow.mV[0] = (2.0f - glow.mV[0]) * SLIDER_SCALE_GLOW_R;
    glow.mV[2] *= SLIDER_SCALE_GLOW_B;

    sky->setGlow(glow);
    sky->update();
}

void APFloaterPhototools::onStarBrightnessChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setStarBrightness((F32)getChild<LLUICtrl>(FIELD_SKY_STAR_BRIGHTNESS)->getValue().asReal());
    sky->update();
}

void APFloaterPhototools::onSunScaleChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setSunScale((F32)(getChild<LLUICtrl>(FIELD_SKY_SUN_SCALE)->getValue().asReal()));
    sky->update();
}

void APFloaterPhototools::onSunAzimElevChanged()
{
    F32 azimuth = (F32)getChild<LLUICtrl>(FIELD_SKY_SUN_AZIMUTH)->getValue().asReal();
    F32 elevation = (F32)getChild<LLUICtrl>(FIELD_SKY_SUN_ELEVATION)->getValue().asReal();

    LLQuaternion quat;

    azimuth *= DEG_TO_RAD;
    elevation *= DEG_TO_RAD;

    if (is_approx_zero(elevation))
    {
        elevation = F_APPROXIMATELY_ZERO;
    }

     quat.setAngleAxis(-elevation, 0, 1, 0);
    LLQuaternion az_quat;
    az_quat.setAngleAxis(F_TWO_PI - azimuth, 0, 0, 1);
    quat *= az_quat;


    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (sky)
    {
        sky->setSunRotation(quat);
        sky->update();
    }
}

void APFloaterPhototools::onMoonImageChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setMoonTextureId(getChild<LLTextureCtrl>(FIELD_SKY_MOON_IMAGE)->getValue().asUUID());
    sky->update();
}

void APFloaterPhototools::onMoonScaleChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setMoonScale((F32)(getChild<LLUICtrl>(FIELD_SKY_MOON_SCALE)->getValue().asReal()));
    sky->update();
}

void APFloaterPhototools::onMoonBrightnessChanged()
{
    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (!sky)
        return;
    sky->setMoonBrightness((F32)(getChild<LLUICtrl>(FIELD_SKY_MOON_BRIGHTNESS)->getValue().asReal()));
    sky->update();
}

void APFloaterPhototools::onMoonAzimElevChanged()
{
    F32 azimuth = (F32)getChild<LLUICtrl>(FIELD_SKY_MOON_AZIMUTH)->getValue().asReal();
    F32 elevation = (F32)getChild<LLUICtrl>(FIELD_SKY_MOON_ELEVATION)->getValue().asReal();
    LLQuaternion quat;

    azimuth *= DEG_TO_RAD;
    elevation *= DEG_TO_RAD;

    if (is_approx_zero(elevation))
    {
        elevation = F_APPROXIMATELY_ZERO;
    }

    quat.setAngleAxis(-elevation, 0, 1, 0);
    LLQuaternion az_quat;
    az_quat.setAngleAxis(F_TWO_PI - azimuth, 0, 0, 1);
    quat *= az_quat;

    // getChild<LLVirtualTrackball>(FIELD_SKY_MOON_ROTATION)->setRotation(quat);

    LLSettingsSky::ptr_t sky = LLEnvironment::instance().getCurrentSky();
    if (sky)
    {
        sky->setMoonRotation(quat);
        sky->update();
    }
}

void APFloaterPhototools::refreshSky()
{
    // Get the current active environment settings
    mLiveSky = LLEnvironment::instance().getCurrentSky();
    mLiveWater = LLEnvironment::instance().getCurrentWater();

    // Determine if the current environment is locally editable
    // This is true only if the selected environment is the user's local override
    bool can_edit = (LLEnvironment::instance().getSelectedEnvironment() == LLEnvironment::ENV_LOCAL);

    // --- Get UI Panels ---
    LLPanel* skyAtmosPanel = getChild<LLPanel>("panel_ap_settings_sky_atmos");
    if (!skyAtmosPanel) { LL_WARNS("PhotoToolsSky") << "refreshSky: Could not find Sky settings panel 'panel_ap_settings_sky_atmos'." << LL_ENDL; }

    LLPanel* skyCloudsPanel = getChild<LLPanel>("panel_ap_settings_sky_clouds");
    if (!skyCloudsPanel) { LL_WARNS("PhotoToolsSky") << "refreshSky: Could not find Sky settings panel 'panel_ap_settings_sky_clouds'." << LL_ENDL; }

    LLPanel* skyHbodiesPanel = getChild<LLPanel>("panel_ap_settings_sky_hbodies");
    if (!skyHbodiesPanel) { LL_WARNS("PhotoToolsSky") << "refreshSky: Could not find Sky settings panel 'panel_ap_settings_sky_hbodies'." << LL_ENDL; }

    LLPanel* waterPanel = getChild<LLPanel>("panel_ap_settings_water");
    if (!waterPanel) { LL_WARNS("PhotoToolsWater") << "refreshSky: Could not find Water settings panel 'panel_ap_settings_water'." << LL_ENDL; }


    // --- Refresh Sky Controls ---
    if (!mLiveSky)
    {
        if (skyAtmosPanel) skyAtmosPanel->setEnabled(false);
        if (skyCloudsPanel) skyCloudsPanel->setEnabled(false);
        if (skyHbodiesPanel) skyHbodiesPanel->setEnabled(false);
        LL_DEBUGS("PhotoToolsSky") << "refreshSky: mLiveSky is null, disabling sky controls." << LL_ENDL;
    }
    else
    {
        if (skyAtmosPanel) skyAtmosPanel->setEnabled(can_edit);
        if (skyCloudsPanel) skyCloudsPanel->setEnabled(can_edit);
        if (skyHbodiesPanel) skyHbodiesPanel->setEnabled(can_edit);

        // Atmospheric Colors
        LLColorSwatchCtrl* clrAmbient = getChild<LLColorSwatchCtrl>(FIELD_SKY_AMBIENT_LIGHT);
        if (clrAmbient) { clrAmbient->set(mLiveSky->getAmbientColor() / SLIDER_SCALE_SUN_AMBIENT); clrAmbient->setEnabled(can_edit); }

        LLColorSwatchCtrl* clrBlueHorizon = getChild<LLColorSwatchCtrl>(FIELD_SKY_BLUE_HORIZON);
        if (clrBlueHorizon) { clrBlueHorizon->set(mLiveSky->getBlueHorizon() / SLIDER_SCALE_BLUE_HORIZON_DENSITY); clrBlueHorizon->setEnabled(can_edit); }

        LLColorSwatchCtrl* clrBlueDensity = getChild<LLColorSwatchCtrl>(FIELD_SKY_BLUE_DENSITY);
        if (clrBlueDensity) { clrBlueDensity->set(mLiveSky->getBlueDensity() / SLIDER_SCALE_BLUE_HORIZON_DENSITY); clrBlueDensity->setEnabled(can_edit); }

        // Atmosphere Settings
        LLUICtrl* hazeHorizonCtrl = getChild<LLUICtrl>(FIELD_SKY_HAZE_HORIZON);
        if (hazeHorizonCtrl) { hazeHorizonCtrl->setValue(mLiveSky->getHazeHorizon()); hazeHorizonCtrl->setEnabled(can_edit); }

        LLUICtrl* hazeDensityCtrl = getChild<LLUICtrl>(FIELD_SKY_HAZE_DENSITY);
        if (hazeDensityCtrl) { hazeDensityCtrl->setValue(mLiveSky->getHazeDensity()); hazeDensityCtrl->setEnabled(can_edit); }

        LLUICtrl* densityMultipCtrl = getChild<LLUICtrl>(FIELD_SKY_DENSITY_MULTIP);
        if (densityMultipCtrl) { F32 density_mult = mLiveSky->getDensityMultiplier() / SLIDER_SCALE_DENSITY_MULTIPLIER; densityMultipCtrl->setValue(density_mult); densityMultipCtrl->setEnabled(can_edit); }

        LLUICtrl* distanceMultipCtrl = getChild<LLUICtrl>(FIELD_SKY_DISTANCE_MULTIP);
        if (distanceMultipCtrl) { distanceMultipCtrl->setValue(mLiveSky->getDistanceMultiplier()); distanceMultipCtrl->setEnabled(can_edit); }

        LLUICtrl* maxAltCtrl = getChild<LLUICtrl>(FIELD_SKY_MAX_ALT);
        if (maxAltCtrl) { maxAltCtrl->setValue(mLiveSky->getMaxY()); maxAltCtrl->setEnabled(can_edit); }

        LLUICtrl* sceneGammaCtrl = getChild<LLUICtrl>(FIELD_SKY_SCENE_GAMMA);
        if (sceneGammaCtrl) { sceneGammaCtrl->setValue(mLiveSky->getGamma()); sceneGammaCtrl->setEnabled(can_edit); }

        LLUICtrl* rpAmbianceCtrl = getChild<LLUICtrl>(FIELD_REFLECTION_PROBE_AMBIANCE);
        static LLCachedControl<bool> should_auto_adjust(gSavedSettings, "RenderSkyAutoAdjustLegacy", false);
        if (rpAmbianceCtrl) { F32 rp_ambiance = mLiveSky->getReflectionProbeAmbiance(should_auto_adjust); rpAmbianceCtrl->setValue(rp_ambiance); rpAmbianceCtrl->setEnabled(can_edit); }

        // updateGammaLabel(mLiveSky); // Keep or adapt this call

        // Rainbow and Halo Settings
        LLUICtrl* moistureCtrl = getChild<LLUICtrl>(FIELD_SKY_DENSITY_MOISTURE_LEVEL);
        if (moistureCtrl) { moistureCtrl->setValue(mLiveSky->getSkyMoistureLevel()); moistureCtrl->setEnabled(can_edit); }

        LLUICtrl* dropletCtrl = getChild<LLUICtrl>(FIELD_SKY_DENSITY_DROPLET_RADIUS);
        if (dropletCtrl) { dropletCtrl->setValue(mLiveSky->getSkyDropletRadius()); dropletCtrl->setEnabled(can_edit); }

        LLUICtrl* iceCtrl = getChild<LLUICtrl>(FIELD_SKY_DENSITY_ICE_LEVEL);
        if (iceCtrl) { iceCtrl->setValue(mLiveSky->getSkyIceLevel()); iceCtrl->setEnabled(can_edit); }

        // Cloud Settings
        LLColorSwatchCtrl* clrCloud = getChild<LLColorSwatchCtrl>(FIELD_SKY_CLOUD_COLOR);
        if (clrCloud) { clrCloud->set(mLiveSky->getCloudColor()); clrCloud->setEnabled(can_edit); }

        LLUICtrl* cloudCoverageCtrl = getChild<LLUICtrl>(FIELD_SKY_CLOUD_COVERAGE);
        if (cloudCoverageCtrl) { cloudCoverageCtrl->setValue(mLiveSky->getCloudShadow()); cloudCoverageCtrl->setEnabled(can_edit); }

        LLUICtrl* cloudScaleCtrl = getChild<LLUICtrl>(FIELD_SKY_CLOUD_SCALE);
        if (cloudScaleCtrl) { cloudScaleCtrl->setValue(mLiveSky->getCloudScale()); cloudScaleCtrl->setEnabled(can_edit); }

        LLUICtrl* cloudVarianceCtrl = getChild<LLUICtrl>(FIELD_SKY_CLOUD_VARIANCE);
        if (cloudVarianceCtrl) { cloudVarianceCtrl->setValue(mLiveSky->getCloudVariance()); cloudVarianceCtrl->setEnabled(can_edit); }

        LLUICtrl* cloudDensityX = getChild<LLUICtrl>(FIELD_SKY_CLOUD_DENSITY_X);
        LLUICtrl* cloudDensityY = getChild<LLUICtrl>(FIELD_SKY_CLOUD_DENSITY_Y);
        LLUICtrl* cloudDensityD = getChild<LLUICtrl>(FIELD_SKY_CLOUD_DENSITY_D);
        LLVector3 cloudDensity(mLiveSky->getCloudPosDensity1().getValue());
        if (cloudDensityX) { cloudDensityX->setValue(cloudDensity[0]); cloudDensityX->setEnabled(can_edit); }
        if (cloudDensityY) { cloudDensityY->setValue(cloudDensity[1]); cloudDensityY->setEnabled(can_edit); }
        if (cloudDensityD) { cloudDensityD->setValue(cloudDensity[2]); cloudDensityD->setEnabled(can_edit); }

        LLUICtrl* cloudDetailX = getChild<LLUICtrl>(FIELD_SKY_CLOUD_DETAIL_X);
        LLUICtrl* cloudDetailY = getChild<LLUICtrl>(FIELD_SKY_CLOUD_DETAIL_Y);
        LLUICtrl* cloudDetailD = getChild<LLUICtrl>(FIELD_SKY_CLOUD_DETAIL_D);
        LLVector3 cloudDetail(mLiveSky->getCloudPosDensity2().getValue());
        if (cloudDetailX) { cloudDetailX->setValue(cloudDetail[0]); cloudDetailX->setEnabled(can_edit); }
        if (cloudDetailY) { cloudDetailY->setValue(cloudDetail[1]); cloudDetailY->setEnabled(can_edit); }
        if (cloudDetailD) { cloudDetailD->setValue(cloudDetail[2]); cloudDetailD->setEnabled(can_edit); }

        LLUICtrl* cloudScrollXY = getChild<LLUICtrl>(FIELD_SKY_CLOUD_SCROLL_XY);
        if (cloudScrollXY) { cloudScrollXY->setValue(mLiveSky->getCloudScrollRate().getValue()); cloudScrollXY->setEnabled(can_edit); }

        LLTextureCtrl* cloudMapCtrl = getChild<LLTextureCtrl>(FIELD_SKY_CLOUD_MAP);
        if (cloudMapCtrl) { cloudMapCtrl->setValue(mLiveSky->getCloudNoiseTextureId()); cloudMapCtrl->setEnabled(can_edit); }

        // Sun and Moon Colors
        LLColorSwatchCtrl* clrSunMoon = getChild<LLColorSwatchCtrl>(FIELD_SKY_SUN_COLOR);
        if (clrSunMoon) { clrSunMoon->set(mLiveSky->getSunlightColor() / SLIDER_SCALE_SUN_AMBIENT); clrSunMoon->setEnabled(can_edit); }

        // Sun and Stars Settings
        LLTextureCtrl* txtSunImage = getChild<LLTextureCtrl>(FIELD_SKY_SUN_IMAGE);
        if (txtSunImage) { txtSunImage->setValue(mLiveSky->getSunTextureId()); txtSunImage->setEnabled(can_edit); }

        LLUICtrl* glowSizeCtrl = getChild<LLUICtrl>(FIELD_SKY_GLOW_SIZE);
        LLUICtrl* glowFocusCtrl = getChild<LLUICtrl>(FIELD_SKY_GLOW_FOCUS);
        LLColor3 glow = mLiveSky->getGlow();
        if (glowSizeCtrl) { glowSizeCtrl->setValue(2.0 - (glow.mV[0] / SLIDER_SCALE_GLOW_R)); glowSizeCtrl->setEnabled(can_edit); }
        if (glowFocusCtrl) { glowFocusCtrl->setValue(glow.mV[2] / SLIDER_SCALE_GLOW_B); glowFocusCtrl->setEnabled(can_edit); }

        LLUICtrl* starBrightnessCtrl = getChild<LLUICtrl>(FIELD_SKY_STAR_BRIGHTNESS);
        if (starBrightnessCtrl) { starBrightnessCtrl->setValue(mLiveSky->getStarBrightness()); starBrightnessCtrl->setEnabled(can_edit); }

        LLUICtrl* sunScaleCtrl = getChild<LLUICtrl>(FIELD_SKY_SUN_SCALE);
        if (sunScaleCtrl) { sunScaleCtrl->setValue(mLiveSky->getSunScale()); sunScaleCtrl->setEnabled(can_edit); }

        LLUICtrl* sunAzimuthCtrl = getChild<LLUICtrl>(FIELD_SKY_SUN_AZIMUTH);
        LLUICtrl* sunElevationCtrl = getChild<LLUICtrl>(FIELD_SKY_SUN_ELEVATION);
        F32 sunAzimuth, sunElevation;
        LLVirtualTrackball::getAzimuthAndElevationDeg(mLiveSky->getSunRotation(), sunAzimuth, sunElevation);
        if (sunAzimuthCtrl) { sunAzimuthCtrl->setValue(sunAzimuth); sunAzimuthCtrl->setEnabled(can_edit); }
        if (sunElevationCtrl) { sunElevationCtrl->setValue(sunElevation); sunElevationCtrl->setEnabled(can_edit); }

        // Moon Settings
        LLTextureCtrl* txtMoonImage = getChild<LLTextureCtrl>(FIELD_SKY_MOON_IMAGE);
        if (txtMoonImage) { txtMoonImage->setValue(mLiveSky->getMoonTextureId()); txtMoonImage->setEnabled(can_edit); }

        LLUICtrl* moonScaleCtrl = getChild<LLUICtrl>(FIELD_SKY_MOON_SCALE);
        if (moonScaleCtrl) { moonScaleCtrl->setValue(mLiveSky->getMoonScale()); moonScaleCtrl->setEnabled(can_edit); }

        LLUICtrl* moonBrightnessCtrl = getChild<LLUICtrl>(FIELD_SKY_MOON_BRIGHTNESS);
        if (moonBrightnessCtrl) { moonBrightnessCtrl->setValue(mLiveSky->getMoonBrightness()); moonBrightnessCtrl->setEnabled(can_edit); }

        LLUICtrl* moonAzimuthCtrl = getChild<LLUICtrl>(FIELD_SKY_MOON_AZIMUTH);
        LLUICtrl* moonElevationCtrl = getChild<LLUICtrl>(FIELD_SKY_MOON_ELEVATION);
        F32 moonAzimuth, moonElevation;
        LLVirtualTrackball::getAzimuthAndElevationDeg(mLiveSky->getMoonRotation(), moonAzimuth, moonElevation);
        if (moonAzimuthCtrl) { moonAzimuthCtrl->setValue(moonAzimuth); moonAzimuthCtrl->setEnabled(can_edit); }
        if (moonElevationCtrl) { moonElevationCtrl->setValue(moonElevation); moonElevationCtrl->setEnabled(can_edit); }

        // Note: Virtual trackball controls FIELD_SKY_SUN_ROTATION and FIELD_SKY_MOON_ROTATION
        // might also need to be refreshed and enabled/disabled here if they exist in your XML.
        // LLVirtualTrackball* sunTrackball = getChild<LLVirtualTrackball>(FIELD_SKY_SUN_ROTATION);
        // if (sunTrackball) { sunTrackball->setRotation(mLiveSky->getSunRotation()); sunTrackball->setEnabled(can_edit); }
        // LLVirtualTrackball* moonTrackball = getChild<LLVirtualTrackball>(FIELD_SKY_MOON_ROTATION);
        // if (moonTrackball) { moonTrackball->setRotation(mLiveSky->getMoonRotation()); moonTrackball->setEnabled(can_edit); }
    }

    // --- Refresh Water Controls ---
    if (!mLiveWater)
    {
        if (waterPanel) waterPanel->setEnabled(false);
        LL_DEBUGS("PhotoToolsWater") << "refreshSky: mLiveWater is null, disabling water controls." << LL_ENDL;
    }
    else
    {
        if (waterPanel) waterPanel->setEnabled(can_edit);

        // Now refresh individual controls within the water panel, checking each pointer
        LLColorSwatchCtrl* clrFog = getChild<LLColorSwatchCtrl>(FIELD_WATER_FOG_COLOR); // **CORRECTED TYPE**
        if (clrFog) { clrFog->set(mLiveWater->getWaterFogColor()); clrFog->setEnabled(can_edit); }

        LLSliderCtrl* sldFogDensity = getChild<LLSliderCtrl>(FIELD_WATER_FOG_DENSITY); // **CORRECTED TYPE**
        if (sldFogDensity) { sldFogDensity->setValue(mLiveWater->getWaterFogDensity()); sldFogDensity->setEnabled(can_edit); }

        LLSliderCtrl* sldUnderwaterMod = getChild<LLSliderCtrl>(FIELD_WATER_UNDERWATER_MOD); // **CORRECTED TYPE**
        if (sldUnderwaterMod) { sldUnderwaterMod->setValue(mLiveWater->getFogMod()); sldUnderwaterMod->setEnabled(can_edit); }

        LLTextureCtrl* txtNormalMap = getChild<LLTextureCtrl>(FIELD_WATER_NORMAL_MAP); // Already correct type
        if (txtNormalMap) { txtNormalMap->setValue(mLiveWater->getNormalMapID()); txtNormalMap->setEnabled(can_edit); }

        LLUICtrl* xyWave1 = getChild<LLUICtrl>(FIELD_WATER_WAVE1_XY); // LLXYVector is LLUICtrl - already correct
        if (xyWave1) { LLVector2 vect2(mLiveWater->getWave1Dir().getValue()); vect2 *= -1.0; xyWave1->setValue(vect2.getValue()); xyWave1->setEnabled(can_edit); }

        LLUICtrl* xyWave2 = getChild<LLUICtrl>(FIELD_WATER_WAVE2_XY); // LLXYVector is LLUICtrl - already correct
        if (xyWave2) { LLVector2 vect2(mLiveWater->getWave2Dir().getValue()); vect2 *= -1.0; xyWave2->setValue(vect2.getValue()); xyWave2->setEnabled(can_edit); }

        LLSliderCtrl* sldNormalScaleX = getChild<LLSliderCtrl>(FIELD_WATER_NORMAL_SCALE_X); // **CORRECTED TYPE**
        LLSliderCtrl* sldNormalScaleY = getChild<LLSliderCtrl>(FIELD_WATER_NORMAL_SCALE_Y); // **CORRECTED TYPE**
        LLSliderCtrl* sldNormalScaleZ = getChild<LLSliderCtrl>(FIELD_WATER_NORMAL_SCALE_Z); // **CORRECTED TYPE**
        LLVector3 vect3(mLiveWater->getNormalScale().getValue());
        if (sldNormalScaleX) { sldNormalScaleX->setValue(vect3[0]); sldNormalScaleX->setEnabled(can_edit); }
        if (sldNormalScaleY) { sldNormalScaleY->setValue(vect3[1]); sldNormalScaleY->setEnabled(can_edit); }
        if (sldNormalScaleZ) { sldNormalScaleZ->setValue(vect3[2]); sldNormalScaleZ->setEnabled(can_edit); }

        LLSliderCtrl* sldFresnelScale = getChild<LLSliderCtrl>(FIELD_WATER_FRESNEL_SCALE); // **CORRECTED TYPE**
        if (sldFresnelScale) { sldFresnelScale->setValue(mLiveWater->getFresnelScale()); sldFresnelScale->setEnabled(can_edit); }

        LLSliderCtrl* sldFresnelOffset = getChild<LLSliderCtrl>(FIELD_WATER_FRESNEL_OFFSET); // **CORRECTED TYPE**
        if (sldFresnelOffset) { sldFresnelOffset->setValue(mLiveWater->getFresnelOffset()); sldFresnelOffset->setEnabled(can_edit); }

        LLSliderCtrl* sldScaleAbove = getChild<LLSliderCtrl>(FIELD_WATER_SCALE_ABOVE); // **CORRECTED TYPE**
        if (sldScaleAbove) { sldScaleAbove->setValue(mLiveWater->getScaleAbove()); sldScaleAbove->setEnabled(can_edit); }

        LLSliderCtrl* sldScaleBelow = getChild<LLSliderCtrl>(FIELD_WATER_SCALE_BELOW); // **CORRECTED TYPE**
        if (sldScaleBelow) { sldScaleBelow->setValue(mLiveWater->getScaleBelow()); sldScaleBelow->setEnabled(can_edit); }

        LLSliderCtrl* sldBlurMultip = getChild<LLSliderCtrl>(FIELD_WATER_BLUR_MULTIP); // **CORRECTED TYPE**
        if (sldBlurMultip) { sldBlurMultip->setValue(mLiveWater->getBlurMultiplier()); sldBlurMultip->setEnabled(can_edit); }
    }
}

void APFloaterPhototools::onChangeLumWeightRSlider()
{
    LLVector3 currentWeights = gSavedSettings.getVector3("APLuminanceWeights");
    F32 newValue = mSliderLumWeightR->getValueF32(); // Get value from slider
    currentWeights.mV[0] = newValue;                  // Update Red component
    mSpinnerLumWeightR->setValue(newValue);           // Sync spinner
    gSavedSettings.setVector3("APLuminanceWeights", currentWeights); // Save setting
}

void APFloaterPhototools::onChangeLumWeightRSpinner()
{
    LLVector3 currentWeights = gSavedSettings.getVector3("APLuminanceWeights");
    F32 newValue = mSpinnerLumWeightR->getValueF32(); // Get value from spinner
    currentWeights.mV[0] = newValue;                  // Update Red component
    mSliderLumWeightR->setValue(newValue);            // Sync slider
    gSavedSettings.setVector3("APLuminanceWeights", currentWeights); // Save setting
}

void APFloaterPhototools::onClickResetLumWeightR()
{
    LLVector3 currentWeights = gSavedSettings.getVector3("APLuminanceWeights");
    F32 defaultValue = 0.299f; // Default Red weight
    currentWeights.mV[0] = defaultValue;                  // Update Red component
    mSpinnerLumWeightR->setValue(defaultValue);           // Sync spinner
    mSliderLumWeightR->setValue(defaultValue);            // Sync slider
    gSavedSettings.setVector3("APLuminanceWeights", currentWeights); // Save setting
}

void APFloaterPhototools::onChangeLumWeightGSlider()
{
    LLVector3 currentWeights = gSavedSettings.getVector3("APLuminanceWeights");
    F32 newValue = mSliderLumWeightG->getValueF32(); // Get value from slider
    currentWeights.mV[1] = newValue;                  // Update Green component
    mSpinnerLumWeightG->setValue(newValue);           // Sync spinner
    gSavedSettings.setVector3("APLuminanceWeights", currentWeights); // Save setting
}

void APFloaterPhototools::onChangeLumWeightGSpinner()
{
    LLVector3 currentWeights = gSavedSettings.getVector3("APLuminanceWeights");
    F32 newValue = mSpinnerLumWeightG->getValueF32(); // Get value from spinner
    currentWeights.mV[1] = newValue;                  // Update Green component
    mSliderLumWeightG->setValue(newValue);            // Sync slider
    gSavedSettings.setVector3("APLuminanceWeights", currentWeights); // Save setting
}

void APFloaterPhototools::onClickResetLumWeightG()
{
    LLVector3 currentWeights = gSavedSettings.getVector3("APLuminanceWeights");
    F32 defaultValue = 0.587f; // Default Green weight
    currentWeights.mV[1] = defaultValue;                  // Update Green component
    mSpinnerLumWeightG->setValue(defaultValue);           // Sync spinner
    mSliderLumWeightG->setValue(defaultValue);            // Sync slider
    gSavedSettings.setVector3("APLuminanceWeights", currentWeights); // Save setting
}

void APFloaterPhototools::onChangeLumWeightBSlider()
{
    LLVector3 currentWeights = gSavedSettings.getVector3("APLuminanceWeights");
    F32 newValue = mSliderLumWeightB->getValueF32(); // Get value from slider
    currentWeights.mV[2] = newValue;                  // Update Blue component
    mSpinnerLumWeightB->setValue(newValue);           // Sync spinner
    gSavedSettings.setVector3("APLuminanceWeights", currentWeights); // Save setting
}

void APFloaterPhototools::onChangeLumWeightBSpinner()
{
    LLVector3 currentWeights = gSavedSettings.getVector3("APLuminanceWeights");
    F32 newValue = mSpinnerLumWeightB->getValueF32(); // Get value from spinner
    currentWeights.mV[2] = newValue;                  // Update Blue component
    mSliderLumWeightB->setValue(newValue);            // Sync slider
    gSavedSettings.setVector3("APLuminanceWeights", currentWeights); // Save setting
}

void APFloaterPhototools::onClickResetLumWeightB()
{
    LLVector3 currentWeights = gSavedSettings.getVector3("APLuminanceWeights");
    F32 defaultValue = 0.114f; // Default Blue weight
    currentWeights.mV[2] = defaultValue;                  // Update Blue component
    mSpinnerLumWeightB->setValue(defaultValue);           // Sync spinner
    mSliderLumWeightB->setValue(defaultValue);            // Sync slider
    gSavedSettings.setVector3("APLuminanceWeights", currentWeights); // Save setting
}

void APFloaterPhototools::onChangeUIScaleSlider()
{
    if (!mUIScaleSlider || !mUIScaleSpinner) return; // Safety check

    F32 newValue = mUIScaleSlider->getValueF32();
    newValue = llclamp(newValue, mUIScaleSlider->getMinValue(), mUIScaleSlider->getMaxValue()); // Clamp value

    mUIScaleSpinner->setValue(newValue); // Sync spinner

    // Immediately apply the setting change
    gSavedSettings.setF32("UIScaleFactor", newValue);

    // <AP:WW> **** Trigger the UI update ****
    if (gViewerWindow) // Safety check for the global pointer
    {
        gViewerWindow->requestResolutionUpdate();
    }
}

void APFloaterPhototools::onChangeUIScaleSpinner()
{
    if (!mUIScaleSlider || !mUIScaleSpinner) return; // Safety check

    F32 newValue = mUIScaleSpinner->getValueF32();
    newValue = llclamp(newValue, mUIScaleSpinner->getMinValue(), mUIScaleSpinner->getMaxValue()); // Clamp value

    mUIScaleSlider->setValue(newValue); // Sync slider

    // Immediately apply the setting change
    gSavedSettings.setF32("UIScaleFactor", newValue);

    // <AP:WW> **** Trigger the UI update ****
    if (gViewerWindow) // Safety check for the global pointer
    {
        gViewerWindow->requestResolutionUpdate();
    }
}

void APFloaterPhototools::onClickResetUIScale()
{
    // 1. Get the default value for UIScaleFactor
    F32 default_scale = 1.0f; // Standard default
    LLControlVariable* ui_scale_ctrl = gSavedSettings.getControl("UIScaleFactor");
    if (ui_scale_ctrl)
    {
        // Try to get the defined default, fallback to 1.0 if needed
        default_scale = static_cast<F32>(ui_scale_ctrl->getDefault().asReal()); 
    }

    // 2. Set the setting to the default value
    gSavedSettings.setF32("UIScaleFactor", default_scale);

    // 3. Sync the UI controls (Slider and Spinner)
    if (mUIScaleSlider)
    {
        mUIScaleSlider->setValue(default_scale);
    }
    if (mUIScaleSpinner)
    {
        mUIScaleSpinner->setValue(default_scale);
    }

    // 4. **** Manually call the update ****
    if (gViewerWindow) // Safety check for the global pointer
    {
        gViewerWindow->requestResolutionUpdate();
    }
}

void APFloaterPhototools::onChangeQuality(const LLSD& data)
{
    U32 level = (U32)(data.asInteger()); // Use asInteger() for direct value lookup if values are sequential integers
    LLFeatureManager::getInstance()->setGraphicsLevel(level, true);

    // --- CRUCIAL: Update the UI ---
    // refreshEnabledGraphics(); // Update enabled/disabled state of controls
    refreshSettings();        // Update values shown in controls (assuming this function exists and is sufficient)
    // Or potentially call a more specific 'refresh()' if needed.
}

// <AP:WW> Placeholder - You MUST implement this function or similar logic
// void APFloaterPhototools::refreshEnabledGraphics()
// {
    // LL_WARNS("Phototools") << "APFloaterPhototools::refreshEnabledGraphics() needs implementation!" << LL_ENDL;
    // Example Logic (needs to be adapted based on actual controls and feature checks):
    // bool shadows_enabled = LLFeatureManager::getInstance()->isFeatureEnabled(LLFeature::Shadows);
    // getChild<LLSlider>("SB_Shd_Clarity")->setEnabled(shadows_enabled);
    // getChild<LLSpinCtrl>("S_Shd_Clarity")->setEnabled(shadows_enabled);
    // ... enable/disable other controls based on features like SSAO, DoF, SSR, Mirrors etc. ...

    // Also update the combo box itself in case the applied level differs
    // (e.g., due to hardware limitations forcing a lower level)
    // if (mGraphicsPresetCombo)
    // {
         // mGraphicsPresetCombo->selectByValue(gSavedSettings.getU32("RenderQualityPerformance"));
    // }
// }

void APFloaterPhototools::saveGraphicsPreset(const LLSD& user_data)
{
    // Simple version: pass the parameter string directly
    LL_DEBUGS("PresetDebug") << "Save Graphics Preset called with parameter: " << user_data.asString() << LL_ENDL;
    LLFloaterReg::showInstance("save_pref_preset", user_data.asString());
}

void APFloaterPhototools::loadGraphicsPreset(const LLSD& user_data)
{
    // Simple version: pass the parameter string directly
    LL_DEBUGS("PresetDebug") << "Load Graphics Preset called with parameter: " << user_data.asString() << LL_ENDL;
    LLFloaterReg::showInstance("load_pref_preset", user_data.asString());
}

void APFloaterPhototools::deleteGraphicsPreset(const LLSD& user_data)
{
    // Simple version: pass the parameter string directly
    LL_DEBUGS("PresetDebug") << "Delete Graphics Preset called with parameter: " << user_data.asString() << LL_ENDL;
    LLFloaterReg::showInstance("delete_pref_preset", user_data.asString());
}

void APFloaterPhototools::onColorBalanceModeChanged()
{
    S32 newMode = gSavedSettings.getS32("APColorBalanceMode");
    if (newMode != mCurrentColorBalanceMode)
    {
        mCurrentColorBalanceMode = newMode;
        refreshColorBalanceControls(); // Update sliders to show values for the new mode
    }
}

void APFloaterPhototools::refreshColorBalanceControls()
{
    F32 valR = 0.0f, valG = 0.0f, valB = 0.0f;

    // Read the correct settings based on the current mode
    switch (mCurrentColorBalanceMode)
    {
        case 0: // Shadows
            valR = gSavedSettings.getF32("APRenderColorBalanceShadsRed");
            valG = gSavedSettings.getF32("APRenderColorBalanceShadsGreen");
            valB = gSavedSettings.getF32("APRenderColorBalanceShadsBlue");
            break;
        case 1: // Midtones (Default case)
        default:
            valR = gSavedSettings.getF32("APRenderColorBalanceMidsRed");
            valG = gSavedSettings.getF32("APRenderColorBalanceMidsGreen");
            valB = gSavedSettings.getF32("APRenderColorBalanceMidsBlue");
            break;
        case 2: // Highlights (Lites)
            valR = gSavedSettings.getF32("APRenderColorBalanceLitesRed");
            valG = gSavedSettings.getF32("APRenderColorBalanceLitesGreen");
            valB = gSavedSettings.getF32("APRenderColorBalanceLitesBlue");
            break;
    }

    // Update the UI controls (ensure they exist first)
    if (mSliderCB_CyanRed && mSpinnerCB_CyanRed) {
        mSliderCB_CyanRed->setValue(valR);
        mSpinnerCB_CyanRed->setValue(valR);
    }
    if (mSliderCB_MagentaGreen && mSpinnerCB_MagentaGreen) {
        mSliderCB_MagentaGreen->setValue(valG);
        mSpinnerCB_MagentaGreen->setValue(valG);
    }
    if (mSliderCB_YellowBlue && mSpinnerCB_YellowBlue) {
        mSliderCB_YellowBlue->setValue(valB);
        mSpinnerCB_YellowBlue->setValue(valB);
    }
}

void APFloaterPhototools::onColorBalanceSliderChanged(LLUICtrl* ctrl, const LLSD& value)
{
    LLSlider* slider = dynamic_cast<LLSlider*>(ctrl);
    if (!slider) return; // Should not happen with our binding

    F32 newValue = slider->getValueF32();
    std::string sliderName = slider->getName();
    LLSpinCtrl* syncSpinner = nullptr;
    std::string settingKey = "";

    // Determine which setting to update and which spinner to sync
    if (sliderName == "SB_CB_CyanRed") {
        syncSpinner = mSpinnerCB_CyanRed;
        switch (mCurrentColorBalanceMode) {
            case 0: settingKey = "APRenderColorBalanceShadsRed"; break;
            case 1: settingKey = "APRenderColorBalanceMidsRed"; break;
            case 2: settingKey = "APRenderColorBalanceLitesRed"; break;
        }
    } else if (sliderName == "SB_CB_MagentaGreen") {
        syncSpinner = mSpinnerCB_MagentaGreen;
         switch (mCurrentColorBalanceMode) {
            case 0: settingKey = "APRenderColorBalanceShadsGreen"; break;
            case 1: settingKey = "APRenderColorBalanceMidsGreen"; break;
            case 2: settingKey = "APRenderColorBalanceLitesGreen"; break;
        }
    } else if (sliderName == "SB_CB_YellowBlue") {
        syncSpinner = mSpinnerCB_YellowBlue;
         switch (mCurrentColorBalanceMode) {
            case 0: settingKey = "APRenderColorBalanceShadsBlue"; break;
            case 1: settingKey = "APRenderColorBalanceMidsBlue"; break;
            case 2: settingKey = "APRenderColorBalanceLitesBlue"; break;
        }
    }

    // Update the setting and sync the spinner if found
    if (!settingKey.empty()) {
        gSavedSettings.setF32(settingKey, newValue);
        if (syncSpinner) {
            syncSpinner->setValue(newValue);
        }
    }
}

void APFloaterPhototools::onColorBalanceSpinnerChanged(LLUICtrl* ctrl, const LLSD& value)
{
    LLSpinCtrl* spinner = dynamic_cast<LLSpinCtrl*>(ctrl);
    if (!spinner) return;

    F32 newValue = spinner->getValueF32();
    std::string spinnerName = spinner->getName();
    LLSlider* syncSlider = nullptr;
    std::string settingKey = "";

    // Determine which setting to update and which slider to sync
    if (spinnerName == "S_CB_CyanRed") {
        syncSlider = mSliderCB_CyanRed;
        switch (mCurrentColorBalanceMode) {
            case 0: settingKey = "APRenderColorBalanceShadsRed"; break;
            case 1: settingKey = "APRenderColorBalanceMidsRed"; break;
            case 2: settingKey = "APRenderColorBalanceLitesRed"; break;
        }
    } else if (spinnerName == "S_CB_MagentaGreen") {
        syncSlider = mSliderCB_MagentaGreen;
        switch (mCurrentColorBalanceMode) {
            case 0: settingKey = "APRenderColorBalanceShadsGreen"; break;
            case 1: settingKey = "APRenderColorBalanceMidsGreen"; break;
            case 2: settingKey = "APRenderColorBalanceLitesGreen"; break;
        }
    } else if (spinnerName == "S_CB_YellowBlue") {
        syncSlider = mSliderCB_YellowBlue;
        switch (mCurrentColorBalanceMode) {
            case 0: settingKey = "APRenderColorBalanceShadsBlue"; break;
            case 1: settingKey = "APRenderColorBalanceMidsBlue"; break;
            case 2: settingKey = "APRenderColorBalanceLitesBlue"; break;
        }
    }

    // Update the setting and sync the slider if found
    if (!settingKey.empty()) {
        gSavedSettings.setF32(settingKey, newValue);
        if (syncSlider) {
            syncSlider->setValue(newValue);
        }
    }
}

void APFloaterPhototools::onClickResetColorBalance(LLUICtrl* ctrl, const LLSD& value)
{
    LLButton* button = dynamic_cast<LLButton*>(ctrl);
    if (!button) return;

    F32 defaultValue = 0.0f; // Default for all color balance sliders is 0
    std::string buttonName = button->getName();
    LLSlider* syncSlider = nullptr;
    LLSpinCtrl* syncSpinner = nullptr;
    std::string settingKey = "";

    // Determine which setting/controls to reset
    if (buttonName == "Reset_CB_CyanRed") {
        syncSlider = mSliderCB_CyanRed;
        syncSpinner = mSpinnerCB_CyanRed;
        switch (mCurrentColorBalanceMode) {
            case 0: settingKey = "APRenderColorBalanceShadsRed"; break;
            case 1: settingKey = "APRenderColorBalanceMidsRed"; break;
            case 2: settingKey = "APRenderColorBalanceLitesRed"; break;
        }
    } else if (buttonName == "Reset_CB_MagentaGreen") {
        syncSlider = mSliderCB_MagentaGreen;
        syncSpinner = mSpinnerCB_MagentaGreen;
        switch (mCurrentColorBalanceMode) {
            case 0: settingKey = "APRenderColorBalanceShadsGreen"; break;
            case 1: settingKey = "APRenderColorBalanceMidsGreen"; break;
            case 2: settingKey = "APRenderColorBalanceLitesGreen"; break;
        }
    } else if (buttonName == "Reset_CB_YellowBlue") {
        syncSlider = mSliderCB_YellowBlue;
        syncSpinner = mSpinnerCB_YellowBlue;
        switch (mCurrentColorBalanceMode) {
            case 0: settingKey = "APRenderColorBalanceShadsBlue"; break;
            case 1: settingKey = "APRenderColorBalanceMidsBlue"; break;
            case 2: settingKey = "APRenderColorBalanceLitesBlue"; break;
        }
    }

    // Reset the setting and sync the controls if found
    if (!settingKey.empty()) {
        gSavedSettings.setF32(settingKey, defaultValue);
        if (syncSlider) {
            syncSlider->setValue(defaultValue);
        }
        if (syncSpinner) {
            syncSpinner->setValue(defaultValue);
        }
    }
}

void APFloaterPhototools::switchViews(ECameraControlMode mode)
{

    // Ensure panel pointers are valid before using
    if (!mPanelPresetViews || !mPanelCameraModes || !mPanelZoomControls ||
        !mBtnCamPresetsView || !mBtnCamModesView || !mBtnCamPanView)
    {
        LL_WARNS("PhotoToolsCamera") << "Camera view control pointers not initialized!" << LL_ENDL;
        return;
    }

    bool show_presets = (mode == CAMERA_CTRL_MODE_PRESETS);
    bool show_modes = (mode == CAMERA_CTRL_MODE_MODES);
    // Assume PAN mode shows the main Orbit/Zoom/Pan controls
    bool show_zoom_pan = (mode == CAMERA_CTRL_MODE_PAN);

    mPanelPresetViews->setVisible(show_presets);
    mPanelCameraModes->setVisible(show_modes);
    mPanelZoomControls->setVisible(show_zoom_pan);

    mBtnCamPresetsView->setToggleState(show_presets);
    mBtnCamModesView->setToggleState(show_modes);
    mBtnCamPanView->setToggleState(show_zoom_pan);

    // Note: Unlike LLFloaterCamera, we might not need to manage mCurrMode/mPrevMode
    // unless other logic depends on it. This function purely handles UI visibility for now.
    // If needed, add state management:
    // mCurrMode = mode;
}

F32 APFloaterPhototools::getCameraOrbitRate(F32 time)
{
    // Using constants defined in unnamed namespace at top of file
    if( time < NUDGE_TIME )
    {
        // Linear ramp up during nudge time
        F32 rate = ORBIT_NUDGE_RATE + time * (1.0f - ORBIT_NUDGE_RATE) / NUDGE_TIME;
        return rate;
    }
    else
    {
        // Full rate after nudge time
        return 1.0f;
    }
}

void APFloaterPhototools::onCameraZoomPlusHeldDown()
{
    // Ensure controls exist
    if (!mBtnZoomPlus || !mSliderZoomDistance) return;

    F32 val = mSliderZoomDistance->getValueF32();
    // Use a small default increment suitable for zoom fraction (0-1 range)
    F32 inc = 0.01f;
    F32 new_val = llclamp(val - inc, 0.0f, 1.0f); // Zoom in moves towards 0
    mSliderZoomDistance->setValue(new_val); // Update slider visually

    // Apply zoom to camera and unlock view
    F32 time = mBtnZoomPlus->getHeldDownTime();
    gAgentCamera.unlockView(); // IMPORTANT: Unlock camera from preset view
    gAgentCamera.setOrbitInKey(getCameraOrbitRate(time)); // Use orbit key for zoom button
}

void APFloaterPhototools::onCameraZoomMinusHeldDown()
{
    // Ensure controls exist
    if (!mBtnZoomMinus || !mSliderZoomDistance) return;

    F32 val = mSliderZoomDistance->getValueF32();
    F32 inc = 0.01f; // Default increment
    F32 new_val = llclamp(val + inc, 0.0f, 1.0f); // Zoom out moves towards 1
    mSliderZoomDistance->setValue(new_val); // Update slider visually

    // Apply zoom to camera and unlock view
    F32 time = mBtnZoomMinus->getHeldDownTime();
    gAgentCamera.unlockView(); // IMPORTANT: Unlock camera from preset view
    gAgentCamera.setOrbitOutKey(getCameraOrbitRate(time)); // Use orbit key for zoom button
}

void APFloaterPhototools::onCameraZoomSliderChanged()
{
    // Ensure control exists
    if (!mSliderZoomDistance) return;

    F32 zoom_level = mSliderZoomDistance->getValueF32();
    zoom_level = llclamp(zoom_level, 0.0f, 1.0f); // Ensure value is valid

    // Apply zoom fraction and unlock view
    gAgentCamera.setCameraZoomFraction(zoom_level);
    gAgentCamera.unlockView(); // IMPORTANT: Unlock camera from preset view
}

void APFloaterPhototools::onCameraRollLeftHeldDown()
{
    // Ensure control exists
    if (!mBtnRollLeft) return;

    F32 time = mBtnRollLeft->getHeldDownTime();
    gAgentCamera.unlockView(); // IMPORTANT: Unlock camera from preset view
    gAgentCamera.setRollLeftKey(getCameraOrbitRate(time));
}

void APFloaterPhototools::onCameraRollRightHeldDown()
{
    // Ensure control exists
    if (!mBtnRollRight) return;

    F32 time = mBtnRollRight->getHeldDownTime();
    gAgentCamera.unlockView(); // IMPORTANT: Unlock camera from preset view
    gAgentCamera.setRollRightKey(getCameraOrbitRate(time));
}

void APFloaterPhototools::onCameraTrackJoystick()
{
    gAgentCamera.unlockView();
    // Optional: LLFirstUse::viewPopup( false ); // Dismiss camera hint if shown
}

void APFloaterPhototools::onCameraRotateJoystick()
{
    gAgentCamera.unlockView();
    // Optional: LLFirstUse::viewPopup( false ); // Dismiss camera hint if shown
}

void APFloaterPhototools::refreshCameraControls()
{
    // Refresh zoom distance slider based on actual camera state
    if (mSliderZoomDistance)
    {
        F32 current_cam_zoom = gAgentCamera.getCameraZoomFraction();
        // Check if update is needed to avoid potential feedback loops
        // (comparing floats requires care)
        if (fabs(mSliderZoomDistance->getValueF32() - current_cam_zoom) > F_APPROXIMATELY_ZERO)
        {
             mSliderZoomDistance->setValue(current_cam_zoom);
        }
    }

    // Add refresh logic for joysticks if needed (e.g., visual state)
    // if (mCamRotateStick) mCamRotateStick->visualRefresh(); // Example: Fictional function
    // if (mCamTrackStick) mCamTrackStick->visualRefresh(); // Example: Fictional function
}

void APFloaterPhototools::onClickCameraItemHandler(const LLSD& userdata)
{
    std::string name = userdata.asString();
    LL_INFOS("PhotoToolsCamera") << "onClickCameraItemHandler called with parameter: [" << name << "]" << LL_ENDL;

    // Check if this is one of the standard preset names that should load a file
    bool is_standard_preset = (name == "Front View" || name == "Side View" || name == "Rear View");

    // --- Actions based on name ---
    if ("mouselook_view" == name)
    {
        LL_INFOS("PhotoToolsCamera") << "Executing: Mouselook View" << LL_ENDL;
        clear_camera_tool(); // Deactivate free cam if active
        gAgentCamera.changeCameraToMouselook();
    }
    else if ("object_view" == name) // <<< FIX: Implement Object View Toggle
    {
        LL_INFOS("PhotoToolsCamera") << "Executing: Object View Toggle" << LL_ENDL;
        LLToolMgr* tool_mgr = LLToolMgr::getInstance();
        LLTool* cam_tool = LLToolCamera::getInstance();
        if (cam_tool && tool_mgr->getCurrentTool() == cam_tool)
        {
            clear_camera_tool(); // Deactivate if active
        }
        else
        {
            activate_camera_tool(); // Activate if not active
        }
    }
    else if (is_standard_preset) // Handle Front, Side, Rear
    {
        LL_INFOS("PhotoToolsCamera") << "Executing: Standard Preset [" << name << "]" << LL_ENDL;
        clear_camera_tool(); // Deactivate free cam if active

        // 1. Switch internal camera logic mode (CHECK YOUR XML PARAMETERS IF THIS IS STILL SWAPPED)
        if ("Front View" == name) { gAgentCamera.switchCameraPreset(CAMERA_PRESET_FRONT_VIEW); }
        else if ("Side View" == name) { gAgentCamera.switchCameraPreset(CAMERA_PRESET_GROUP_VIEW); }
        else if ("Rear View" == name) { gAgentCamera.switchCameraPreset(CAMERA_PRESET_REAR_VIEW); }

        // 2. Load the corresponding preset file settings
        if (gSavedSettings.getString("PresetCameraActive") != name)
        {
            LLPresetsManager::getInstance()->loadPreset(PRESETS_CAMERA, name);
        }

        // 3. Reset zoom
        gAgentCamera.resetCameraZoomFraction();
    }
    else
    {
        LL_WARNS("PhotoToolsCamera") << "Unhandled CameraPresets.ChangeView parameter: " << name << LL_ENDL;
    }

    // Update the selection highlights AFTER changing state
    updateCameraItemsSelection();

    // Ensure UI reflects loaded settings
    refreshSettings();
}

void APFloaterPhototools::onShowCameraPresetsFloater()
{
    LL_DEBUGS("PhotoToolsCamera") << "Showing camera_presets floater." << LL_ENDL;
    LLFloaterReg::showInstance("camera_presets", LLSD(), false); // Replicates original action
}

void APFloaterPhototools::updateCameraItemsSelection()
{
    LLPanelCameraItem* rear_view_item = findChild<LLPanelCameraItem>("rear_view");
    LLPanelCameraItem* group_view_item = findChild<LLPanelCameraItem>("group_view");
    LLPanelCameraItem* front_view_item = findChild<LLPanelCameraItem>("front_view");
    LLPanelCameraItem* mouselook_view_item = findChild<LLPanelCameraItem>("mouselook_view");
    LLPanelCameraItem* object_view_item = findChild<LLPanelCameraItem>("object_view");

    if (!rear_view_item || !group_view_item || !front_view_item || !mouselook_view_item || !object_view_item)
    {
        LL_WARNS("PhotoToolsCamera") << "updateCameraItemsSelection: Could not find one or more camera items." << LL_ENDL;
        return;
    }

    ECameraPreset preset = (ECameraPreset) gSavedSettings.getU32("CameraPresetType");

    // <<< FIX: Check actual tool state for free cam active
    LLToolMgr* tool_mgr = LLToolMgr::getInstance();
    LLTool* cam_tool = LLToolCamera::getInstance();
    bool is_free_cam_active = (cam_tool && tool_mgr->getCurrentTool() == cam_tool);

    bool is_mouselook_active = (gAgentCamera.getCameraMode() == CAMERA_MODE_MOUSELOOK);

    LLSD argument;

    argument["selected"] = (!is_free_cam_active && !is_mouselook_active && preset == CAMERA_PRESET_REAR_VIEW);
    rear_view_item->setValue(argument);

    argument["selected"] = (!is_free_cam_active && !is_mouselook_active && preset == CAMERA_PRESET_GROUP_VIEW);
    group_view_item->setValue(argument);

    argument["selected"] = (!is_free_cam_active && !is_mouselook_active && preset == CAMERA_PRESET_FRONT_VIEW);
    front_view_item->setValue(argument);

    argument["selected"] = is_mouselook_active;
    mouselook_view_item->setValue(argument);

    argument["selected"] = is_free_cam_active; // Highlight based on actual tool state
    object_view_item->setValue(argument);
}

// Corrected onStoreCameraView
void APFloaterPhototools::onStoreCameraView(S32 slot_index)
{
    // --- NEW: Check if Flycam is active ---
    LLViewerJoystick* joystick = LLViewerJoystick::getInstance();
    bool is_flycam_active = joystick && joystick->getOverrideCamera();

    if (is_flycam_active)
    {
        FSCommon::report_to_nearby_chat("Cannot save Viewer Camera preset while Flycam is active.");
        // LL_WARNS if you want to keep it:
        // LL_WARNS("PhotoToolsCamera") << "Attempted to save Viewer preset in Flycam mode for slot " << slot_index << LL_ENDL;
        return;
    }
    // --- End NEW Check ---

    // --- Existing Viewer Preset Save Logic ---
    // LL_WARNS if you want to keep it:
    // LL_WARNS("PhotoToolsCamera") << "onStoreCameraView (Standard Save) Slot " << slot_index << ":" << LL_ENDL;

    LLVector3d current_pos_global = gAgentCamera.getCameraPositionGlobal();
    LLVector3d current_focus_global = gAgentCamera.getFocusTargetGlobal();
    LLUUID focus_object_id = LLUUID::null;
    if (gAgentCamera.getFocusObject()) { // Safer check
        focus_object_id = gAgentCamera.getFocusObject()->getID();
    }
    // *** ADD THIS LINE TO GET THE ROLL ***
    F32 current_roll = gAgentCamera.getRollAngle();

    // LL_WARNS if you want them:
    // LL_WARNS("PhotoToolsCamera") << "  - Storing Pos (AgentCam): " << current_pos_global << LL_ENDL;
    // LL_WARNS("PhotoToolsCamera") << "  - Storing Focus (AgentCam): " << current_focus_global << LL_ENDL;
    // LL_WARNS("PhotoToolsCamera") << "  - Storing Focus Obj ID: " << focus_object_id.asString() << LL_ENDL;
    // LL_WARNS("PhotoToolsCamera") << "  - Storing Roll (AgentCam): " << current_roll << LL_ENDL; // Log the roll

    // Format slot index
    std::ostringstream slot_stream;
    slot_stream << std::setw(2) << std::setfill('0') << slot_index;
    std::string slot_str = slot_stream.str();

    // Define Setting Keys
    std::string key_pos = "APStoredCameraPos_" + slot_str;
    std::string key_focus = "APStoredCameraFocus_" + slot_str;
    std::string key_focus_id = "APStoredCameraFocusObjectId_" + slot_str;
    // *** ADD THIS LINE FOR THE ROLL KEY ***
    std::string key_roll = "APStoredCameraRoll_" + slot_str;

    // Declare Settings (Ideally done once at startup)
    gSavedPerAccountSettings.declareVec3d(key_pos, LLVector3d::zero, "Aperture Camera Pos Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);
    gSavedPerAccountSettings.declareVec3d(key_focus, LLVector3d::zero, "Aperture Camera Focus Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);
    gSavedPerAccountSettings.declareString(key_focus_id, LLUUID::null.asString(), "Aperture Camera Focus Object ID Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);
    // *** ADD THIS LINE TO DECLARE THE ROLL SETTING ***
    gSavedPerAccountSettings.declareF32(key_roll, 0.0f, "Aperture Camera Roll Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);

    // Save Values
    gSavedPerAccountSettings.setVector3d(key_pos, current_pos_global);
    gSavedPerAccountSettings.setVector3d(key_focus, current_focus_global);
    gSavedPerAccountSettings.setString(key_focus_id, focus_object_id.asString());
    // *** ADD THIS LINE TO SAVE THE ROLL VALUE ***
    gSavedPerAccountSettings.setF32(key_roll, current_roll);

    // LL_WARNS if you want it:
    // LL_WARNS("PhotoToolsCamera") << "Stored Viewer Camera Pos/Focus/Roll for slot " << slot_index << LL_ENDL;

    // User Feedback
    std::string message = "Viewer Camera preset saved to slot " + std::to_string(slot_index) + ".";
    FSCommon::report_to_nearby_chat(message);
}

void APFloaterPhototools::onLoadCameraView(S32 slot_index)
{
    // Format slot index with leading zero
    std::ostringstream slot_stream;
    slot_stream << std::setw(2) << std::setfill('0') << slot_index;
    std::string slot_str = slot_stream.str();

    // Construct the setting keys with "AP" prefix
    std::string key_pos = "APStoredCameraPos_" + slot_str;
    std::string key_focus = "APStoredCameraFocus_" + slot_str;
    std::string key_objid = "APStoredCameraFocusObjectId_" + slot_str;
    std::string key_roll = "APStoredCameraRoll_" + slot_str;

    // --- Dynamically declare controls before getting (ensures they exist, returning default if not set) ---
    gSavedPerAccountSettings.declareVec3d(key_pos, LLVector3d::zero, "Aperture Camera Pos Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);
    gSavedPerAccountSettings.declareVec3d(key_focus, LLVector3d::zero, "Aperture Camera Focus Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);
    gSavedPerAccountSettings.declareString(key_objid, LLUUID::null.asString(), "Aperture Camera ObjID Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);
    gSavedPerAccountSettings.declareF32(key_roll, 0.0f, "Aperture Camera Roll Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);

    // --- Load Values from Per-Account Settings ---
    LLVector3d stored_camera_pos = gSavedPerAccountSettings.getVector3d(key_pos);
    LLVector3d stored_camera_focus = gSavedPerAccountSettings.getVector3d(key_focus);
    LLUUID stored_camera_focus_object_id(gSavedPerAccountSettings.getString(key_objid));
    F32 stored_camera_roll = gSavedPerAccountSettings.getF32(key_roll);

    // --- Check if Slot has actually been saved ---
    if (stored_camera_focus_object_id.isNull() && stored_camera_pos.isExactlyZero() && (stored_camera_roll == 0.0f))
    {
        if (stored_camera_focus.isExactlyZero())
        {
             std::string message = "Camera slot " + std::to_string(slot_index) + " appears empty.";
             FSCommon::report_to_nearby_chat(message);
             LL_WARNS("PhotoToolsCamera") << "Attempted to load empty camera slot " << slot_index << " (default values found)." << LL_ENDL;
             return;
        }
    }

    // --- Sanity/Distance Check ---
    F32 renderFarClip = gSavedSettings.getF32("RenderFarClip");
    F32 far_clip_squared = renderFarClip * renderFarClip;

    if (dist_vec_squared(gAgent.getPositionGlobal(), stored_camera_pos) > far_clip_squared)
    {
        std::string msg = LLTrans::getString("LoadCameraPositionOutsideDrawDistance");
        if (msg == "LoadCameraPositionOutsideDrawDistance") msg = "Stored camera position is outside your draw distance.";
        FSCommon::report_to_nearby_chat(msg);
        LL_WARNS("PhotoToolsCamera") << "Camera slot " << slot_index << " is outside draw distance." << LL_ENDL;
        return;
    }

    // --- Handle Flycam ---
    if (LLViewerJoystick::getInstance()->getOverrideCamera())
    {
        LLViewerJoystick::getInstance()->setOverrideCamera(false); // Uses the correct setter
        LLViewerJoystick::getInstance()->setCameraNeedsUpdate(true);
    }

    // --- Apply Loaded State ---
    gAgentCamera.unlockView(); // Ensure camera is free first

    // Apply Roll FIRST
    gAgentCamera.setRollAngle(stored_camera_roll); // Uses public setter

    // Then apply Position and Focus
    gAgentCamera.setCameraPosAndFocusGlobal(stored_camera_pos, stored_camera_focus, stored_camera_focus_object_id);


    // --- User Feedback ---
    std::string message = "Camera view loaded from slot " + std::to_string(slot_index) + ".";
    FSCommon::report_to_nearby_chat(message);

    LL_DEBUGS("PhotoToolsCamera") << "Loaded camera view from slot " << slot_index << LL_ENDL;

    // Refresh UI elements that depend on camera state immediately
    refreshCameraControls();
}


void APFloaterPhototools::onStoreFlycamView(S32 slot_index)
{
    LL_WARNS("PhotoToolsCamera") << "onStoreFlycamView (V20+Roll Save) Slot " << slot_index << ":" << LL_ENDL;

    // --- Check Flycam Active ---
    // V20 didn't explicitly check, but good practice
    LLViewerJoystick* joystick = LLViewerJoystick::getInstance();
    bool is_flycam_active = joystick && joystick->getOverrideCamera();
    if (!is_flycam_active)
    {
        LL_WARNS("PhotoToolsCamera") << "Store failed: Flycam not active for slot " << slot_index << LL_ENDL;
        FSCommon::report_to_nearby_chat("Cannot save Flycam preset: Flycam is not active.");
        return;
    }
    // --- End Check ---

    // --- Get Data Exactly as Specified in V20 ---
    LLVector3d current_pos_global = gAgentCamera.getCameraPositionGlobal();
    LLVector3 current_at_axis = LLViewerCamera::getInstance()->getAtAxis();

    // *** NEW: Get Roll from the final camera orientation ***
    LLQuaternion current_orientation = LLViewerCamera::getInstance()->getQuaternion();
    F32 current_roll = 0.0f; // Default to 0
    F32 pitch_unused, yaw_unused; // We only need roll
    current_orientation.getEulerAngles(&current_roll, &pitch_unused, &yaw_unused); // Correct use of pointers
    // *** End NEW Roll Code ***

    LL_WARNS("PhotoToolsCamera") << "  - Storing Pos (AgentCam): " << current_pos_global << LL_ENDL;
    LL_WARNS("PhotoToolsCamera") << "  - Storing AtAxis (ViewerCam): " << current_at_axis << LL_ENDL;
    LL_WARNS("PhotoToolsCamera") << "  - Storing Roll (ViewerCam Quat): " << current_roll << LL_ENDL; // Log roll

    // 1. Format slot index
    std::ostringstream slot_stream;
    slot_stream << std::setw(2) << std::setfill('0') << slot_index;
    std::string slot_str = slot_stream.str();

    // 2. Define Setting Keys (Add Roll)
    std::string key_pos = "APFlycamPos_" + slot_str;
    std::string key_at_axis = "APFlycamAtAxis_" + slot_str;
    std::string key_roll = "APFlycamRoll_" + slot_str; // NEW Key for Roll

    // 3. Declare Settings (Add Roll)
    // Note: Declaring here ensures they exist, but ideally declare once at startup.
    gSavedPerAccountSettings.declareVec3d(key_pos, LLVector3d::zero, "Aperture Flycam Pos Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);
    gSavedPerAccountSettings.declareVec3(key_at_axis, LLVector3::x_axis, "Aperture Flycam AtAxis Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);
    gSavedPerAccountSettings.declareF32(key_roll, 0.0f, "Aperture Flycam Roll Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS); // NEW Declare Roll

    // 4. Save Values (Add Roll)
    gSavedPerAccountSettings.setVector3d(key_pos, current_pos_global);
    gSavedPerAccountSettings.setVector3(key_at_axis, current_at_axis);
    gSavedPerAccountSettings.setF32(key_roll, current_roll); // NEW Save Roll

    LL_WARNS("PhotoToolsCamera") << "Stored Flycam Pos, AtAxis, and Roll for slot " << slot_index << LL_ENDL;

    // 5. User Feedback (Updated Message)
    std::string message = "Flycam Camera Pos/AtAxis/Roll saved to slot " + std::to_string(slot_index) + ".";
    FSCommon::report_to_nearby_chat(message);
}


void APFloaterPhototools::onLoadFlycamView(S32 slot_index)
{
    LL_WARNS("PhotoToolsCamera") << "onLoadFlycamView (V20+Roll Load) Slot " << slot_index << ":" << LL_ENDL;

    // 1. Format slot index
    std::ostringstream slot_stream;
    slot_stream << std::setw(2) << std::setfill('0') << slot_index;
    std::string slot_str = slot_stream.str();

    // 2. Define Setting Keys (Add Roll)
    std::string key_pos = "APFlycamPos_" + slot_str;
    std::string key_at_axis = "APFlycamAtAxis_" + slot_str;
    std::string key_roll = "APFlycamRoll_" + slot_str; // NEW Key for Roll

    // 3. Declare Settings (Add Roll - Needed for getF32 default)
    // Note: Declaring here ensures they exist, but ideally declare once at startup.
    gSavedPerAccountSettings.declareVec3d(key_pos, LLVector3d::zero, "Aperture Flycam Pos Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);
    gSavedPerAccountSettings.declareVec3(key_at_axis, LLVector3::x_axis, "Aperture Flycam AtAxis Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS);
    gSavedPerAccountSettings.declareF32(key_roll, 0.0f, "Aperture Flycam Roll Slot " + slot_str, LLControlVariable::PERSIST_ALWAYS); // NEW Declare Roll

    // 4. Load Values (Add Roll)
    LLVector3d stored_pos = gSavedPerAccountSettings.getVector3d(key_pos);
    LLVector3 stored_at_axis = gSavedPerAccountSettings.getVector3(key_at_axis);
    F32 stored_roll = gSavedPerAccountSettings.getF32(key_roll); // NEW Load Roll (uses declared default if not found)

    LL_WARNS("PhotoToolsCamera") << "  - Loaded Pos: " << stored_pos << LL_ENDL;
    LL_WARNS("PhotoToolsCamera") << "  - Loaded AtAxis: " << stored_at_axis << LL_ENDL;
    LL_WARNS("PhotoToolsCamera") << "  - Loaded Roll: " << stored_roll << LL_ENDL; // Log Roll

    // 5. Check if Empty (Based on Position) - Same as V20
    bool is_empty = stored_pos.isExactlyZero();
    LL_WARNS("PhotoToolsCamera") << "  - Empty check result: " << (is_empty ? "EMPTY" : "NOT EMPTY") << LL_ENDL;
    if (is_empty) {
        std::string message = "Flycam Camera preset slot " + std::to_string(slot_index) + " appears empty.";
        FSCommon::report_to_nearby_chat(message);
        LL_WARNS("PhotoToolsCamera") << "Attempted to load empty Flycam preset slot " << slot_index << "." << LL_ENDL;
        return;
    }

    // 6. Draw Distance Check - Same as V20
    F32 renderFarClip = gSavedSettings.getF32("RenderFarClip");
    F32 far_clip_squared = renderFarClip * renderFarClip;
    if (dist_vec_squared(gAgent.getPositionGlobal(), stored_pos) > far_clip_squared) {
        std::string msg = LLTrans::getString("LoadCameraPositionOutsideDrawDistance");
        if (msg == "LoadCameraPositionOutsideDrawDistance") msg = "Stored camera position is outside your draw distance.";
        FSCommon::report_to_nearby_chat(msg);
        LL_WARNS("PhotoToolsCamera") << "Flycam preset slot " << slot_index << " is outside draw distance." << LL_ENDL;
        return;
    }

    // 7. Handle Flycam Deactivation - Same as V20
    bool was_flycam_active = LLViewerJoystick::getInstance()->getOverrideCamera();
    if (was_flycam_active) {
        LL_WARNS("PhotoToolsCamera") << "Flycam active. Deactivating override." << LL_ENDL;
        LLViewerJoystick::getInstance()->setOverrideCamera(false);
    } else { LL_WARNS("PhotoToolsCamera") << "Flycam inactive." << LL_ENDL; }

    // --- 8. Apply State via LLAgentCamera ---
    LL_WARNS("PhotoToolsCamera") << "  - Step 8: Applying state via LLAgentCamera..." << LL_ENDL;
    gAgentCamera.unlockView();
    gAgentCamera.stopCameraAnimation();

    // 8a. Calculate Focus using LOADED Pos + AtAxis - Same as V20
    const F64 focus_distance = 5.0;
    LLVector3d at_axis_d = LLVector3d(stored_at_axis);
    at_axis_d.normalize();
    LLVector3d calculated_focus_global = stored_pos + at_axis_d * focus_distance;

    // 8b. Set Position & Calculated Focus - Same as V20
    gAgentCamera.setCameraPosAndFocusGlobal(stored_pos, calculated_focus_global, LLUUID::null);
    LL_WARNS("PhotoToolsCamera") << "  - Called setCameraPosAndFocusGlobal. Pos: " << stored_pos << " Focus: " << calculated_focus_global << LL_ENDL;

    // 8c. *** APPLY LOADED ROLL *** (Replaces forcing Roll to ZERO)
    gAgentCamera.setRollAngle(stored_roll); // Use the loaded value
    LL_WARNS("PhotoToolsCamera") << "  - Called setRollAngle with Roll: " << stored_roll << LL_ENDL; // Log the applied roll
    // --- End Apply State ---

    // 9. Force Update Signal - Same as V20
    LLViewerJoystick::getInstance()->setCameraNeedsUpdate(true);

    // 10. User Feedback (Updated Message)
    std::string message = "Flycam Camera preset loaded from slot " + std::to_string(slot_index) + ".";
    if (was_flycam_active) { message += " (Flycam deactivated)"; }
    FSCommon::report_to_nearby_chat(message);
    LL_WARNS("PhotoToolsCamera") << "Loaded Flycam preset view from slot " << slot_index << LL_ENDL;

    // 11. Refresh UI Controls - Same as V20
    refreshCameraControls();
}

void APFloaterPhototools::onWaterFogColorChanged()
{
    // 1. Get the CURRENT active water and sky settings objects
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();

    // Ensure we have valid objects to work with
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterFogColorChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    // Get the control and check it
    LLColorSwatchCtrl* ctrl = getChild<LLColorSwatchCtrl>(FIELD_WATER_FOG_COLOR);
    if (!ctrl) return;

    // 2. Modify the CURRENT water settings object directly
    current_water->setWaterFogColor(LLColor3(ctrl->get()));

    // 3. Explicitly set the LOCAL environment using CURRENT sky and the MODIFIED CURRENT water
    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    // 4. Trigger immediate update
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onWaterFogDensityChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterFogDensityChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLSliderCtrl* ctrl = getChild<LLSliderCtrl>(FIELD_WATER_FOG_DENSITY);
    if (!ctrl) return;

    current_water->setWaterFogDensity(ctrl->getValueF32());

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onWaterUnderwaterModChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterUnderwaterModChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLSliderCtrl* ctrl = getChild<LLSliderCtrl>(FIELD_WATER_UNDERWATER_MOD);
    if (!ctrl) return;

    current_water->setFogMod(ctrl->getValueF32());

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onWaterNormalMapChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterNormalMapChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLTextureCtrl* picker_ctrl = getChild<LLTextureCtrl>(FIELD_WATER_NORMAL_MAP);
    if (!picker_ctrl) return;

    LLUUID new_texture_id = picker_ctrl->getImageAssetID();
    current_water->setNormalMapID(new_texture_id); // Modify the current object

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);

    picker_ctrl->setValue(new_texture_id);
}

void APFloaterPhototools::onWaterLargeWaveChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterLargeWaveChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLUICtrl* ctrl = getChild<LLUICtrl>(FIELD_WATER_WAVE1_XY);
    if (!ctrl) return;

    LLVector2 vect(ctrl->getValue());
    vect *= -1.0; // Flip
    current_water->setWave1Dir(vect);

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onWaterSmallWaveChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterSmallWaveChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLUICtrl* ctrl = getChild<LLUICtrl>(FIELD_WATER_WAVE2_XY);
    if (!ctrl) return;

    LLVector2 vect(ctrl->getValue());
    vect *= -1.0; // Flip
    current_water->setWave2Dir(vect);

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onWaterNormalScaleChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterNormalScaleChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLSliderCtrl* ctrlX = getChild<LLSliderCtrl>(FIELD_WATER_NORMAL_SCALE_X);
    LLSliderCtrl* ctrlY = getChild<LLSliderCtrl>(FIELD_WATER_NORMAL_SCALE_Y);
    LLSliderCtrl* ctrlZ = getChild<LLSliderCtrl>(FIELD_WATER_NORMAL_SCALE_Z);
    if (!ctrlX || !ctrlY || !ctrlZ) return;

    LLVector3 vect(ctrlX->getValueF32(), ctrlY->getValueF32(), ctrlZ->getValueF32());
    current_water->setNormalScale(vect);

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onWaterFresnelScaleChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterFresnelScaleChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLSliderCtrl* ctrl = getChild<LLSliderCtrl>(FIELD_WATER_FRESNEL_SCALE);
    if (!ctrl) return;

    current_water->setFresnelScale(ctrl->getValueF32());

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onWaterFresnelOffsetChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterFresnelOffsetChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLSliderCtrl* ctrl = getChild<LLSliderCtrl>(FIELD_WATER_FRESNEL_OFFSET);
    if (!ctrl) return;

    current_water->setFresnelOffset(ctrl->getValueF32());

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onWaterScaleAboveChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterScaleAboveChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLSliderCtrl* ctrl = getChild<LLSliderCtrl>(FIELD_WATER_SCALE_ABOVE);
    if (!ctrl) return;

    current_water->setScaleAbove(ctrl->getValueF32());

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onWaterScaleBelowChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterScaleBelowChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLSliderCtrl* ctrl = getChild<LLSliderCtrl>(FIELD_WATER_SCALE_BELOW);
    if (!ctrl) return;

    current_water->setScaleBelow(ctrl->getValueF32());

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onWaterBlurMultipChanged()
{
    LLSettingsWater::ptr_t current_water = LLEnvironment::instance().getCurrentWater();
    LLSettingsSky::ptr_t current_sky = LLEnvironment::instance().getCurrentSky();
    if (!current_water || !current_sky)
    {
        LL_WARNS("PhotoToolsWater") << "onWaterBlurMultipChanged: Cannot apply change, current water or sky is null." << LL_ENDL;
        return;
    }

    LLSliderCtrl* ctrl = getChild<LLSliderCtrl>(FIELD_WATER_BLUR_MULTIP);
    if (!ctrl) return;

    current_water->setBlurMultiplier(ctrl->getValueF32());

    LLEnvironment::instance().setEnvironment(LLEnvironment::ENV_LOCAL, current_sky, current_water);
    LLEnvironment::instance().updateEnvironment(LLEnvironment::TRANSITION_INSTANT, true);
}

void APFloaterPhototools::onAPSShowFavoritesBarToggle()
{
    LLCheckBoxCtrl* favBarCheckbox = getChild<LLCheckBoxCtrl>("CH_B_Show_Navbar_Favorites");
    if (favBarCheckbox)
    {
        bool newValue = favBarCheckbox->get();

        gSavedSettings.setBOOL("FSInternalShowNavbarFavoritesPanel", newValue);

        if (gSavedSettings.getBOOL("ShowNavbarFavoritesPanel") != newValue)
        {
            gSavedSettings.setBOOL("ShowNavbarFavoritesPanel", newValue);
        }
    }
}
