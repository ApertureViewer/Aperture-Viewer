/**
 * @file apfloaterphototools.h
 * @brief Phototools panel
 *
 * $LicenseInfo:firstyear=2011&license=viewerlgpl$
 * Copyright (C) 2025, William Weaver (paperwork) @ Second Life
 * Copyright (C) 2011, WoLf Loonie @ Second Life
 * Copyright (C) 2013, Zi Ree @ Second Life
 * Copyright (C) 2013, Ansariel Hiller @ Second Life
 
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

// Header Guard - Prevents the compiler from using this header more than once
#ifndef PHOTOTOOLS_H
#define PHOTOTOOLS_H

// Iclude Directives - tells the compiler other headers that this header needs 
#include "llenvironment.h"
#include "llfloater.h"

#include "llfloatercamera.h"
#include "llpanel.h"

#include "llsettingsbase.h"
#include "llsettingssky.h"

#include "llfeaturemanager.h"

#include "boost/signals2.hpp"

// #include "llquaternion.h"  // Required for quaternion calculations
// #include "llvirtualtrackball.h"
// #include "llsky.h"  // Needed for mLiveSky

// Forward Declarations - declare specific classes that are needed in order to build this header and cpp without have to create a dependnacy on additonal headers to reduce complile time and complexity
class LLCheckBoxCtrl;
class LLComboBox;
class LLLayoutPanel;
class LLLayoutStack;
class LLLineEditor;
class LLSlider;
class LLSliderCtrl;
class LLSpinCtrl;
class LLTextBox;
class LLColorSwatchCtrl;
class LLJoystickCameraRotate;
class LLJoystickCameraTrack;

#define PRESET_NAME_REGION_DEFAULT "__Regiondefault__"
#define PRESET_NAME_DAY_CYCLE "__Day_Cycle__"
#define PRESET_NAME_NONE "__None__"

typedef enum e_phototools_update_param
{
    PT_PARAM_SKY,
    PT_PARAM_WATER,
    // PT_PARAM_DAYCYCLE
} EPhototoolsParam;

class APFloaterPhototools : public LLFloater
{
    friend class LLFloaterReg;

public:
    // --- Public Section ---
    // Methods in this section are accessible from other parts of the code.

    // --- Lifecycle: The constructor and destructor are essential for creating and destroying APFloaterPhototools objects. ---
    APFloaterPhototools(const LLSD& key); // Constructor
    virtual ~APFloaterPhototools();        // Destructor

    // --- Overrides: The postBuild, onOpen, and onClose methods are virtual functions inherited from LLFloater and need to be public so that the viewer framework can call them. ---
    virtual bool postBuild();
    virtual void onOpen(const LLSD& key);
    virtual void onClose(bool app_quitting);

    // --- Environment Preset Management: Methods to set the sky, water, day cycle, and environment, since other parts of the code may call these methods. ---
    void setSelectedSky(const std::string& preset_name);
    void setSelectedWater(const std::string& preset_name);
    // void setSelectedDayCycle(const std::string& preset_name);
    void setSelectedEnvironment();

    // --- Settings Refresh: A method for other code to request the floater be refreshed with new values. ---
    void refreshSettings();

    void refreshColorBalanceControls(); // Updates sliders/spinners based on mCurrentColorBalanceMode

    // General Environment Editing Functions 
    void captureCurrentEnvironment();           // FEA
    void refreshSky();                         // FEA
    void onEnvironmentUpdated(LLEnvironment::EnvSelection_t env, S32 version);  //FEA

    void onAPSShowFavoritesBarToggle();

private:
    // --- Private Section ---
    // Methods and member variables in this section are only accessible from within the APFloaterPhototools class.

    // --- UI Element Declarations: Keep all the LLComboBox, LLSlider, LLSpinCtrl, etc., member variables together. This makes it easier to see what UI elements the class manages. ---
    LLComboBox*         mWLPresetsCombo;
    LLComboBox*         mWaterPresetsCombo;
    // LLComboBox*         mDayCyclePresetsCombo;
    LLSlider*           mSliderRenderShadowSplitExponentY;
    LLSpinCtrl*         mSpinnerRenderShadowSplitExponentY;
    LLSlider*           mSliderRenderShadowGaussianX;
    LLSlider*           mSliderRenderShadowGaussianY;
    LLSpinCtrl*         mSpinnerRenderShadowGaussianX;
    LLSpinCtrl*         mSpinnerRenderShadowGaussianY;
    LLSlider*           mSliderRenderSSAOEffectX;
    LLSpinCtrl*         mSpinnerRenderSSAOEffectX;
    LLSlider*           mSliderRenderSSAOEffectY;
    LLSpinCtrl*         mSpinnerRenderSSAOEffectY;
    LLSliderCtrl*       mAvatarZOffsetSlider;
    LLButton*           mBtnResetDefaults;
    LLSliderCtrl*       mMaxComplexitySlider;
    LLTextBox*          mMaxComplexityLabel;
    LLSlider*           mAnimationSpeedSlider;
    LLUICtrl*           mAnimationSpeedSpinner;
    LLCheckBoxCtrl*     mShowMySettingsCheckbox; 
    LLSlider*           mUIScaleSlider;
    LLSpinCtrl*         mUIScaleSpinner;
    LLButton*           mResetUIScaleBtn;
    LLComboBox*         mGraphicsPresetCombo; // <AP:WW> Pointer for preset combo box

    // --- Environment Management: These member variables are closely related to environment settings. ---
    LLSettingsSky::ptr_t        mLiveSky;
    LLSettingsWater::ptr_t      mLiveWater;
    // LLSettingsDay::ptr_t        mLiveDay;

    // --- Connections and Slots: These member variables are related to signal and slot connections. ---
    LLEnvironment::connection_t mEnvChangedConnection;
    boost::signals2::connection mRegionChangedSlot;

    // --- Preset Loading: All the loadPresets methods are kept together. These are internal methods used to populate the UI and are not intended to be called from outside the class. ---
    void loadPresets();
    // void loadDayCyclePresets(const std::multimap<std::string, LLUUID>& daycycle_map);
    void loadSkyPresets(const std::multimap<std::string, LLUUID>& sky_map);
    void loadWaterPresets(const std::multimap<std::string, LLUUID>& water_map);

    // --- Environment Preset Selection Handlers: All the onChange... methods are kept together. These methods are called when the user interacts with the environment preset combo boxes. ---
    void onChangeWaterPreset();
    void onChangeSkyPreset();
    // void onChangeDayCyclePreset();
    void selectSkyPreset(const LLSD& preset);
    void selectWaterPreset(const LLSD& preset);
    // void selectDayCyclePreset(const LLSD& preset);

    // --- Environment Preset Navigation Handlers: All the onClick... methods for handling the previous and next buttons are grouped together. ---
    void onClickSkyPrev();
    void onClickSkyNext();
    void onClickWaterPrev();
    void onClickWaterNext();
    // void onClickDayCyclePrev();
    // void onClickDayCycleNext();
    void onClickResetToRegionDefault();
    void setDefaultPresetsEnabled(bool enabled);

    // --- UI Control Event Handlers (Shadows Tab): methods that handle the events triggered by changes in the settings on the "Shadows" tab. ---
    void onChangeRenderShadowSplitExponentSliderY();
    void onChangeRenderShadowSplitExponentSpinnerY();
    void onClickResetRenderShadowSplitExponentY();
    void onChangeRenderShadowGaussianSlider();
    void onChangeRenderShadowGaussianSpinner();
    void onClickResetRenderShadowGaussianX();
    void onClickResetRenderShadowGaussianY();
    void onChangeRenderSSAOEffectSlider();
    void onChangeRenderSSAOEffectSpinner();
    void onClickResetRenderSSAOEffectX();
    void onChangeRenderSSAOEffectSliderY();
    void onChangeRenderSSAOEffectSpinnerY();
    void onClickResetRenderSSAOEffectY();

    // --- UI Control Event Handlers (General Tab): methods that handle the events triggered by changes in the settings on the "General" tab. ---
    void onShowMySettingsChanged();

    void onChangeQuality(const LLSD& data); // <AP:WW> Declare the callback

    // --- Complexity Management: All methods to the complexcity management and complexcity label. ---
    void updateMaxNonImpostors(const LLSD& newvalue);
    void updateMaxComplexity();
    void updateMaxComplexityLabel(const LLSD& newvalue);
    void refreshAvatarComplexityControlsFromSetting(const LLSD& new_setting_value_unused);

    // --- Preset Helper Methods (Internal Logic):  Methods that are used internally within the class to set up and manage the floater's functionality. ---
    bool isValidPreset(const LLSD& preset);
    void stepComboBox(LLComboBox* ctrl, bool forward);
    void initCallbacks(); 

    // Evironment Preset Editing Fuctions
    // Function order matchs UI structure

    // Atmospheric Colors
    void onAmbientLightChanged();               // FEA
    void onBlueHorizonChanged();                // FEA
    void onBlueDensityChanged();                // FEA

    // Atmosphere Settings
    void onHazeHorizonChanged();                // PES
    void onHazeDensityChanged();                // PES
    void onDensityMultipChanged();              // PES
    void onDistanceMultipChanged();             // PES
    void onMaxAltChanged();                     // PES
    void onSceneGammaChanged();                 // FEA
    void updateGammaLabel(LLSettingsSky::ptr_t sky);                // FEA (I think this is UI for Brightness)
    void onReflectionProbeAmbianceChanged();    // FEA

    // Rainbow and Halo Settings
    void onMoistureLevelChanged();              // PES
    void onDropletRadiusChanged();              // PES
    void onIceLevelChanged();                   // PES

    // Cloud Settings
    void onCloudColorChanged();                 // FEA
    void onCloudCoverageChanged();              // FEA
    void onCloudScaleChanged();                 // FEA
    void onCloudVarianceChanged();              // PES
    void onCloudDensityChanged();               // PES
    void onCloudDetailChanged();                // PES
    void onCloudScrollChanged();                // PES
    void onCloudMapChanged();                   // FEA

    // Sun and Moon Colors
    void onSunColorChanged();                   // FEA

    // Sun and Stars Settings
    void onSunImageChanged();                   // PES
    void onGlowChanged();                       // FES
    void onStarBrightnessChanged();             // FES
    void onSunScaleChanged();                   // FES
    void onSunAzimElevChanged();                // FES

    // Moon Settings
    void onMoonImageChanged();                  // PES
    void onMoonScaleChanged();                  // PES
    void onMoonBrightnessChanged();             // PES
    void onMoonAzimElevChanged();               // FEA

    // <AP:WW> ADD START: Water Settings Event Handlers (Declarations) - KEEP THESE
    void onWaterFogColorChanged();
    void onWaterFogDensityChanged();
    void onWaterUnderwaterModChanged(); // Renamed from onFogUnderWaterChanged for clarity and consistency
    void onWaterNormalMapChanged();

    void onWaterLargeWaveChanged(); // Renamed from onLargeWaveChanged for clarity
    void onWaterSmallWaveChanged(); // Renamed from onSmallWaveChanged for clarity

    void onWaterNormalScaleChanged();

    void onWaterFresnelScaleChanged();
    void onWaterFresnelOffsetChanged();
    void onWaterScaleAboveChanged();
    void onWaterScaleBelowChanged();
    void onWaterBlurMultipChanged();
    // <AP:WW> ADD END: Water Settings Event Handlers (Declarations)


    LLSlider*           mSliderLumWeightR;
    LLSpinCtrl*         mSpinnerLumWeightR;
    LLButton*           mResetLumWeightRBtn;

    LLSlider*           mSliderLumWeightG;
    LLSpinCtrl*         mSpinnerLumWeightG;
    LLButton*           mResetLumWeightGBtn;

    LLSlider*           mSliderLumWeightB;
    LLSpinCtrl*         mSpinnerLumWeightB;
    LLButton*           mResetLumWeightBBtn;
    
    LLSlider*           mSliderCB_CyanRed;
    LLSpinCtrl*         mSpinnerCB_CyanRed;
    LLButton*           mResetBtnCB_CyanRed;

    LLSlider*           mSliderCB_MagentaGreen;
    LLSpinCtrl*         mSpinnerCB_MagentaGreen;
    LLButton*           mResetBtnCB_MagentaGreen;

    LLSlider*           mSliderCB_YellowBlue;
    LLSpinCtrl*         mSpinnerCB_YellowBlue;
    LLButton*           mResetBtnCB_YellowBlue;

    S32                 mCurrentColorBalanceMode; // 0: Shads, 1: Mids, 2: Lites

    // --- Callbacks ---
    void onChangeLumWeightRSlider();
    void onChangeLumWeightRSpinner();
    void onClickResetLumWeightR();

    void onChangeLumWeightGSlider();
    void onChangeLumWeightGSpinner();
    void onClickResetLumWeightG();

    void onChangeLumWeightBSlider();
    void onChangeLumWeightBSpinner();
    void onClickResetLumWeightB();

    void onColorBalanceModeChanged(); // Handler for APColorBalanceMode setting change
    void onColorBalanceSliderChanged(LLUICtrl* ctrl, const LLSD& value); // Unified handler for sliders
    void onColorBalanceSpinnerChanged(LLUICtrl* ctrl, const LLSD& value); // Unified handler for spinners
    void onClickResetColorBalance(LLUICtrl* ctrl, const LLSD& value);    // Unified handler for reset buttons

    void onChangeUIScaleSlider();
    void onChangeUIScaleSpinner();
    void onClickResetUIScale();

    // void refreshEnabledGraphics();         // <AP:WW> To enable/disable controls based on preset
    //  void refresh();                        // <AP:WW> To update control values (maybe refreshSettings() is enough?)

    // --- Helper Methods ---
    // void updateVectorComponent(int index, F32 value); // Helper to update setting

    // Refresh Functions
    // void refreshSunPosition();
    
    // Functions for Spinner Slider Control - Implemetation Post Fuctionality of All UI
    // void onSunAzimuthChanged();
    // void onSunElevationChanged();
    // void onSunAzimuthSpinnerChanged();
    // void onSunElevationSpinnerChanged();

    // --- Graphics Preset Management Callbacks ---
    void saveGraphicsPreset(const LLSD& user_data);
    void loadGraphicsPreset(const LLSD& user_data);
    void deleteGraphicsPreset(const LLSD& user_data);

    // --- Camera Control UI Elements ---
    LLJoystickCameraRotate* mCamRotateStick;
    LLButton*               mBtnRollLeft;
    LLButton*               mBtnRollRight;
    LLButton*               mBtnZoomPlus;
    LLButton*               mBtnZoomMinus;
    LLSlider*               mSliderZoomDistance; // Maps to slider_bar name="zoom_slider"
    LLJoystickCameraTrack*  mCamTrackStick;

    // --- Camera Control Mode Buttons & Panels ---
    LLButton*               mBtnCamPresetsView; // presets_btn
    LLButton*               mBtnCamPanView;     // pan_btn
    LLButton*               mBtnCamModesView;   // avatarview_btn

    LLPanel*                mPanelPresetViews;  // preset_views_list
    LLPanel*                mPanelCameraModes;  // camera_modes_list
    LLPanel*                mPanelZoomControls; // panel name="zoom" containing joysticks/slider

    // --- Camera Control Event Handlers & Helpers ---
    void onCameraZoomPlusHeldDown();
    void onCameraZoomMinusHeldDown();
    void onCameraZoomSliderChanged();
    void onCameraRollLeftHeldDown();
    void onCameraRollRightHeldDown();
    void onCameraTrackJoystick();   // Minimal handler
    void onCameraRotateJoystick();  // Minimal handler

    // Handler for switching camera control views
    void switchViews(ECameraControlMode mode); // Uses ECameraControlMode from llfloatercamera.h

    // Helper for calculating nudge rate
    F32 getCameraOrbitRate(F32 time);

    // Function to refresh camera controls UI from current state/settings
    void refreshCameraControls();

    void updateCameraItemsSelection(); // Updates selection highlight for preset/mode items
    void onClickCameraItemHandler(const LLSD& userdata); // Back to reference
    void onShowCameraPresetsFloater(); // Handles CameraPresets.ShowPresetsList (gear_btn)

    // Dummy handlers for Preset buttons (Part 2) - parameters match expected usage later
    void onStoreCameraView(S32 slot_index); // Handles Camera.StoreViewXX
    void onLoadCameraView(S32 slot_index);  // Handles Camera.LoadViewXX

    // Handlers for Flycam Camera Presets (Uses LLViewerCamera state)
    void onStoreFlycamView(S32 slot_index); // Handles Camera.StoreFlycamViewXX
    void onLoadFlycamView(S32 slot_index);  // Handles Camera.LoadFlycamViewXX

};

#endif 