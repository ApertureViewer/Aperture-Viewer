/**
 * @file llfeaturemanager.cpp
 * @brief LLFeatureManager class implementation
 *
 * $LicenseInfo:firstyear=2003&license=viewerlgpl$
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

#include <iostream>
#include <fstream>

#include <boost/regex.hpp>
#include <boost/assign/list_of.hpp>

#include "llfeaturemanager.h"
#include "lldir.h"

#include "llsys.h"
#include "llgl.h"

#include "llappviewer.h"
#include "llbufferstream.h"
#include "llnotificationsutil.h"
#include "llviewercontrol.h"
#include "llworld.h"
#include "lldrawpoolterrain.h"
#include "llviewertexturelist.h"
#include "llversioninfo.h"
#include "llwindow.h"
#include "llui.h"
#include "llcontrol.h"
#include "llboost.h"
#include "llweb.h"
#include "llviewershadermgr.h"
#include "llstring.h"
#include "stringize.h"
#include "llcorehttputil.h"

#if LL_WINDOWS
#include "lldxhardware.h"
#endif

// <FS:Techwolf Lupindo> downloadable gpu table support
#include "fscommon.h"
#include <boost/filesystem.hpp>
// </FS:Techwolf Lupindo>

#if LL_DARWIN
const char FEATURE_TABLE_FILENAME[] = "featuretable_mac.txt";
#elif LL_LINUX
const char FEATURE_TABLE_FILENAME[] = "featuretable_linux.txt";
#else
const char FEATURE_TABLE_FILENAME[] = "featuretable_aperture.txt";
#endif

#if 0                               // consuming code in #if 0 below
#endif
LLFeatureInfo::LLFeatureInfo(const std::string& name, const bool available, const F32 level)
    : mValid(true), mName(name), mAvailable(available), mRecommendedLevel(level)
{
}

LLFeatureList::LLFeatureList(const std::string& name)
    : mName(name)
{
}

LLFeatureList::~LLFeatureList()
{
}

void LLFeatureList::addFeature(const std::string& name, const bool available, const F32 level)
{
    if (mFeatures.count(name))
    {
        LL_WARNS("RenderInit") << "LLFeatureList::Attempting to add preexisting feature " << name << LL_ENDL;
    }

    LLFeatureInfo fi(name, available, level);
    LL_DEBUGS_ONCE("RenderInit") << "Feature '" << name << "' "
                                 << (available ? "" : "not " ) << "available"
                                 << " at " << level
                                 << LL_ENDL;
    mFeatures[name] = fi;
}

bool LLFeatureList::isFeatureAvailable(const std::string& name)
{
    if (mFeatures.count(name))
    {
        return mFeatures[name].mAvailable;
    }

    LL_WARNS_ONCE("RenderInit") << "Feature " << name << " not on feature list!" << LL_ENDL;

    // changing this to true so you have to explicitly disable
    // something for it to be disabled
    return true;
}

F32 LLFeatureList::getRecommendedValue(const std::string& name)
{
    if (mFeatures.count(name) && isFeatureAvailable(name))
    {
        LL_DEBUGS_ONCE("RenderInit") << "Setting '" << name << "' to recommended value " <<  mFeatures[name].mRecommendedLevel << LL_ENDL;
        return mFeatures[name].mRecommendedLevel;
    }

    LL_WARNS_ONCE("RenderInit") << "Feature " << name << " not on feature list or not available!" << LL_ENDL;
    return 0;
}

bool LLFeatureList::maskList(LLFeatureList &mask)
{
    LL_DEBUGS_ONCE() << "Masking with " << mask.mName << LL_ENDL;
    //
    // Lookup the specified feature mask, and overlay it on top of the
    // current feature mask.
    //

    LLFeatureInfo mask_fi;

    feature_map_t::iterator feature_it;
    for (feature_it = mask.mFeatures.begin(); feature_it != mask.mFeatures.end(); ++feature_it)
    {
        mask_fi = feature_it->second;
        //
        // Look for the corresponding feature
        //
        if (!mFeatures.count(mask_fi.mName))
        {
            LL_WARNS("RenderInit") << "Feature " << mask_fi.mName << " in mask not in top level!" << LL_ENDL;
            continue;
        }

        LLFeatureInfo &cur_fi = mFeatures[mask_fi.mName];
        if (mask_fi.mAvailable && !cur_fi.mAvailable)
        {
            LL_WARNS("RenderInit") << "Mask attempting to reenabling disabled feature, ignoring " << cur_fi.mName << LL_ENDL;
            continue;
        }
        cur_fi.mAvailable = mask_fi.mAvailable;
        cur_fi.mRecommendedLevel = llmin(cur_fi.mRecommendedLevel, mask_fi.mRecommendedLevel);
        LL_DEBUGS("RenderInit") << "Feature mask " << mask.mName
                << " Feature " << mask_fi.mName
                << " Mask: " << mask_fi.mRecommendedLevel
                << " Now: " << cur_fi.mRecommendedLevel << LL_ENDL;
    }

    LL_DEBUGS("RenderInit") << "After applying mask " << mask.mName << std::endl;
        // Will conditionally call dump only if the above message will be logged, thanks
        // to it being wrapped by the LL_DEBUGS and LL_ENDL macros.
        dump();
    LL_CONT << LL_ENDL;

    return true;
}

void LLFeatureList::dump()
{
    LL_DEBUGS("RenderInit") << "Feature list: " << mName << LL_ENDL;

    LLFeatureInfo fi;
    feature_map_t::iterator feature_it;
    for (feature_it = mFeatures.begin(); feature_it != mFeatures.end(); ++feature_it)
    {
        fi = feature_it->second;
        LL_DEBUGS("RenderInit") << "With " << mName << " feature " << fi.mName << " " << fi.mAvailable << ":" << fi.mRecommendedLevel << LL_ENDL;
    }
}

static const std::vector<std::string> sGraphicsLevelNames = boost::assign::list_of
    ("AP_Level_0_Essential_Visuals")   // Level 0
    ("AP_Level_1_HDR_Foundation")      // Level 1
    ("AP_Level_2_Shadows")             // Level 2
    ("AP_Level_3_SSAO")                // Level 3
    ("AP_Level_4_SSR")                 // Level 4
    ("AP_Level_5_DOF")                 // Level 5
    ("AP_Level_6_Mirrors")             // Level 6
    ("AP_Level_7_Rich_Candy")          // Level 7
    ("AP_Level_8_Full_Candy")          // Level 8
;

U32 LLFeatureManager::getMaxGraphicsLevel() const
{
    return static_cast<U32>(sGraphicsLevelNames.size()) - 1;
}

bool LLFeatureManager::isValidGraphicsLevel(U32 level) const
{
    return (level <= getMaxGraphicsLevel());
}

std::string LLFeatureManager::getNameForGraphicsLevel(U32 level) const
{
    if (isValidGraphicsLevel(level))
    {
        return sGraphicsLevelNames[level];
    }
    return STRINGIZE("Invalid graphics level " << level << ", valid are 0 .. "
                     << getMaxGraphicsLevel());
}

S32 LLFeatureManager::getGraphicsLevelForName(const std::string& name) const
{
    const std::string FixedFunction("FixedFunction");
    std::string rname(name);
    if (LLStringUtil::endsWith(rname, FixedFunction))
    {
        // chop off any "FixedFunction" suffix
        rname = rname.substr(0, rname.length() - FixedFunction.length());
    }
    for (S32 i(0), iend(getMaxGraphicsLevel()); i <= iend; ++i)
    {
        if (sGraphicsLevelNames[i] == rname)
        {
            return i;
        }
    }
    return -1;
}

LLFeatureList *LLFeatureManager::findMask(const std::string& name)
{
    if (mMaskList.count(name))
    {
        return mMaskList[name];
    }

    return NULL;
}

bool LLFeatureManager::maskFeatures(const std::string& name)
{
    LLFeatureList *maskp = findMask(name);
    if (!maskp)
    {
        LL_DEBUGS("RenderInit") << "Unknown feature mask " << name << LL_ENDL;
        return false;
    }
    LL_INFOS("RenderInit") << "Applying GPU Feature list: " << name << LL_ENDL;
    return maskList(*maskp);
}

bool LLFeatureManager::loadFeatureTables()
{
    // *TODO - if I or anyone else adds something else to the skipped list
    // make this data driven.  Put it in the feature table and parse it
    // correctly

    // first table is install with app
    std::string app_path = gDirUtilp->getAppRODataDir();
    app_path += gDirUtilp->getDirDelimiter();

    std::string filename;
    filename = FEATURE_TABLE_FILENAME;

    app_path += filename;

    bool parse_ok = parseFeatureTable(app_path);

    return parse_ok;
}


bool LLFeatureManager::parseFeatureTable(std::string filename)
{
    LL_INFOS("RenderInit") << "Attempting to parse feature table from " << filename << LL_ENDL;

    llifstream file;
    std::string name;
    U32     version;

    cleanupFeatureTables(); // in case an earlier attempt left partial results
    file.open(filename.c_str());     /*Flawfinder: ignore*/

    if (!file)
    {
        LL_WARNS("RenderInit") << "Unable to open feature table " << filename << LL_ENDL;
        return false;
    }

    // Check file version
    file >> name;
    if (name != "version")
    {
        LL_WARNS("RenderInit") << filename << " does not appear to be a valid feature table!" << LL_ENDL;
        return false;
    }
    file >> version;

    mTableVersion = version;
    LL_INFOS("RenderInit") << "Found feature table version " << version << LL_ENDL;

    LLFeatureList *flp = NULL;
    bool parse_ok = true;
    while (parse_ok && file >> name )
    {
        char buffer[MAX_STRING];         /*Flawfinder: ignore*/

        if (name.substr(0,2) == "//")
        {
            // This is a comment.
            file.getline(buffer, MAX_STRING);
            continue;
        }

        if (name == "list")
        {
            LL_DEBUGS("RenderInit") << "Before new list" << std::endl;
            if (flp)
            {
                flp->dump();
            }
            else
            {
                LL_CONT << "No current list";
            }
            LL_CONT << LL_ENDL;

            // It's a new mask, create it.
            file >> name;
            if (!mMaskList.count(name))
            {
                flp = new LLFeatureList(name);
                mMaskList[name] = flp;
            }
            else
            {
                LL_WARNS("RenderInit") << "Overriding mask '" << name << "'; this is invalid!" << LL_ENDL;
                parse_ok = false;
            }
        }
        else
        {
            if (flp)
            {
                S32 available;
                F32 recommended;
                file >> available >> recommended;
                flp->addFeature(name, available, recommended);
            }
            else
            {
                LL_WARNS("RenderInit") << "Specified parameter before <list> keyword!" << LL_ENDL;
                parse_ok = false;
            }
        }
    }
    file.close();

    if (!parse_ok)
    {
        LL_WARNS("RenderInit") << "Discarding feature table data from " << filename << LL_ENDL;
        cleanupFeatureTables();
    }

    return parse_ok;
}

F32 gpu_benchmark();

#if LL_WINDOWS

F32 logExceptionBenchmark()
{
    // FIXME: gpu_benchmark uses many C++ classes on the stack to control state.
    //  SEH exceptions with our current exception handling options do not call
    //  destructors for these classes, resulting in an undefined state should
    //  this handler be invoked.
    F32 gbps = -1;
    __try
    {
        gbps = gpu_benchmark();
    }
    __except (msc_exception_filter(GetExceptionCode(), GetExceptionInformation()))
    {
        // HACK - ensure that profiling is disabled
        LLGLSLShader::finishProfile();

        // convert to C++ styled exception
        char integer_string[32];
        sprintf(integer_string, "SEH, code: %lu\n", GetExceptionCode());
        throw std::exception(integer_string);
    }
    return gbps;
}
#endif

// llfeaturemanager.cpp

// ... (other includes and code above) ...

bool LLFeatureManager::loadGPUClass()
{
    // <AP:WW> Force skipping the GPU benchmark for all users always.
    // bool runBenchmark = !gSavedSettings.getBOOL("SkipBenchmark");
    bool runBenchmark = false; // Force false to always skip
    // if (!gSavedSettings.getBOOL("SkipBenchmark"))
    if (runBenchmark)
    // </AP:WW>
    { // START of if (runBenchmark) block
        F32 class1_gbps = gSavedSettings.getF32("RenderClass1MemoryBandwidth");
        //get memory bandwidth from benchmark
        F32 gbps;
        try
        {
            // <FS:ND> Allow to skip gpu_benchmark with -noprobe.
            // This can make sense for some Intel GPUs which can take 15+ Minutes or crash during gpu_benchmark
            gbps = -1.0f;
            // <AP:WW> // Benchmark is already skipped by the forced 'runBenchmark = false' above, no need to check NoHardwareProbe here again.
            // if( !gSavedSettings.getBOOL( "NoHardwareProbe" ) )
            // </AP:WW>
#if LL_WINDOWS
                gbps = logExceptionBenchmark();
#else
                gbps = gpu_benchmark();
#endif
            // </FS:ND>
        }
        catch (const std::exception& e)
        {
            gbps = -1.f;
            LL_WARNS("RenderInit") << "GPU benchmark failed: " << e.what() << LL_ENDL;
        }

        mGPUMemoryBandwidth = gbps;

        // bias by CPU speed
        F32 cpu_basis_mhz = gSavedSettings.getF32("RenderCPUBasis");
        F32 cpu_mhz = (F32) gSysCPU.getMHz();
        F32 cpu_bias = llclamp(cpu_mhz / cpu_basis_mhz, 0.5f, 1.f);
        gbps *= cpu_bias;

        if (gbps < 0.f)
        { //couldn't bench, default to Low
#if LL_DARWIN
        //GLVersion is misleading on OSX, just default to class 3 if we can't bench
        LL_WARNS("RenderInit") << "Unable to get an accurate benchmark; defaulting to class 3" << LL_ENDL;
        mGPUClass = GPU_CLASS_3;
#else
            mGPUClass = GPU_CLASS_0;
#endif
        }
        else if (gbps <= class1_gbps)
        {
            mGPUClass = GPU_CLASS_1;
        }
        // <AP:WW> Removed redundant 'else if' chain as benchmark is skipped </AP:WW>
        // Note: The rest of the 'else if' chain determining GPU class 2-5 is intentionally removed
        // because 'runBenchmark' is always false, so this code path is never taken.
        // We only needed the 'if (gbps < 0.f)' and the 'else if (gbps <= class1_gbps)'
        // for completeness if 'runBenchmark' were ever true again, but even those
        // won't execute with the current modification.

#if LL_WINDOWS
        const F32Gigabytes MIN_PHYSICAL_MEMORY(2);

        LLMemory::updateMemoryInfo();
        F32Gigabytes physical_mem = LLMemory::getMaxMemKB();
        if (MIN_PHYSICAL_MEMORY > physical_mem && mGPUClass > GPU_CLASS_1)
        {
            // reduce quality on systems that don't have enough memory
            mGPUClass = (EGPUClass)(mGPUClass - 1);
        }
#endif //LL_WINDOWS

    } // <----- !!! THIS IS THE MISSING CLOSING BRACE for 'if (runBenchmark)' !!!
    else // START of else block (when benchmark is skipped)
    {
        //setting says don't benchmark MAINT-7558
        LL_WARNS("RenderInit") << "Setting 'SkipBenchmark' is true OR forced skip; defaulting to class 1 (may be required for some GPUs)" << LL_ENDL; // <AP:WW> Added OR forced skip to comment </AP:WW>

        mGPUClass = GPU_CLASS_1;
    } // END of else block

    // defaults
    mGPUString = gGLManager.getRawGLString();
    mGPUSupported = true;

    return true; // indicates that a gpu value was established
} // END of LLFeatureManager::loadGPUClass function

// ... (the rest of the functions like cleanupFeatureTables, initSingleton, etc. should follow *outside* this function) ...

void LLFeatureManager::cleanupFeatureTables()
{
    std::for_each(mMaskList.begin(), mMaskList.end(), DeletePairedPointer());
    mMaskList.clear();
}

void LLFeatureManager::initSingleton()
{
    // load the tables
    loadFeatureTables();

    // get the gpu class
    loadGPUClass();

    // apply the base masks, so we know if anything is disabled
    applyBaseMasks();
}

void LLFeatureManager::applyRecommendedSettings()
{
    // apply saved settings
    // cap the level at 2 (high)
    U32 level = llmax(GPU_CLASS_0, llmin(mGPUClass, GPU_CLASS_5));

    LL_INFOS("RenderInit") << "Applying Recommended Features for level " << level << LL_ENDL;

    setGraphicsLevel(level, false);
    gSavedSettings.setU32("RenderQualityPerformance", level);

    // now apply the tweaks to draw distance
    // these are double negatives, because feature masks only work by
    // downgrading values, so i needed to make a true value go to false
    // for certain cards, thus the awkward name, "Disregard..."
    if(!gSavedSettings.getBOOL("Disregard96DefaultDrawDistance"))
    {
        gSavedSettings.setF32("RenderFarClip", 96.0f);
    }
    else if(!gSavedSettings.getBOOL("Disregard128DefaultDrawDistance"))
    {
        gSavedSettings.setF32("RenderFarClip", 128.0f);
    }
}

void LLFeatureManager::applyFeatures(bool skipFeatures)
{
    // see featuretable.txt / featuretable_linux.txt / featuretable_mac.txt

#ifndef LL_RELEASE_FOR_DOWNLOAD
    dump();
#endif

    // scroll through all of these and set their corresponding control value
    for(feature_map_t::iterator mIt = mFeatures.begin();
        mIt != mFeatures.end();
        ++mIt)
    {
        // skip features you want to skip
        // do this for when you don't want to change certain settings
        if(skipFeatures)
        {
            if(mSkippedFeatures.find(mIt->first) != mSkippedFeatures.end())
            {
                continue;
            }
        }

        // get the control setting
        LLControlVariable* ctrl = gSavedSettings.getControl(mIt->first);
        if(ctrl == NULL)
        {
            LL_WARNS("RenderInit") << "AHHH! Control setting " << mIt->first << " does not exist!" << LL_ENDL;
            continue;
        }

        // handle all the different types
        if(ctrl->isType(TYPE_BOOLEAN))
        {
            gSavedSettings.setBOOL(mIt->first, (bool)getRecommendedValue(mIt->first));
        }
        else if (ctrl->isType(TYPE_S32))
        {
            gSavedSettings.setS32(mIt->first, (S32)getRecommendedValue(mIt->first));
        }
        else if (ctrl->isType(TYPE_U32))
        {
            gSavedSettings.setU32(mIt->first, (U32)getRecommendedValue(mIt->first));
        }
        else if (ctrl->isType(TYPE_F32))
        {
            gSavedSettings.setF32(mIt->first, (F32)getRecommendedValue(mIt->first));
        }
        else
        {
            LL_WARNS("RenderInit") << "AHHH! Control variable is not a numeric type!" << LL_ENDL;
        }
    }
}

void LLFeatureManager::setGraphicsLevel(U32 level, bool skipFeatures)
{
    LLViewerShaderMgr::sSkipReload = true;
    flush_glerror(); // Whatever may have already happened (e.g., to cause us to change), don't let confuse it with new initializations.
    applyBaseMasks();

    // if we're passed an invalid level, default to "Low"
    std::string features(isValidGraphicsLevel(level)? getNameForGraphicsLevel(level) : "Low");

    maskFeatures(features);

    applyFeatures(skipFeatures);

    LLViewerShaderMgr::sSkipReload = false;
    LLViewerShaderMgr::instance()->setShaders();
    gPipeline.refreshCachedSettings();
}

void LLFeatureManager::applyBaseMasks()
{
    // reapply masks
    mFeatures.clear();

    LLFeatureList* maskp = findMask("all");
    if(maskp == NULL)
    {
        LL_WARNS("RenderInit") << "AHH! No \"all\" in feature table!" << LL_ENDL;
        return;
    }

    mFeatures = maskp->getFeatures();

    // mask class
    if (mGPUClass >= 0 && mGPUClass < 6)
    {
        const char* class_table[] =
        {
            "Class0",
            "Class1",
            "Class2",
            "Class3",
            "Class4",
            "Class5",
        };

        LL_INFOS("RenderInit") << "Setting GPU Class to " << class_table[mGPUClass] << LL_ENDL;
        maskFeatures(class_table[mGPUClass]);
    }
    else
    {
        LL_INFOS("RenderInit") << "Setting GPU Class to Unknown" << LL_ENDL;
        maskFeatures("Unknown");
    }

    // now all those wacky ones
    if (gGLManager.mIsNVIDIA)
    {
        maskFeatures("NVIDIA");
    }
    if (gGLManager.mIsAMD)
    {
        maskFeatures("AMD");
    }
    if (gGLManager.mIsIntel)
    {
        maskFeatures("Intel");

        static constexpr F32 TARGET_GL_VERSION =
#if LL_DARWIN
            4.09f;
#else
            4.59f;
#endif

        // check against 3.33 to avoid applying this fallback twice
        if (gGLManager.mGLVersion < TARGET_GL_VERSION && gGLManager.mGLVersion > 3.33f)
        {
            // if we don't have OpenGL 4.6 on intel, set it to OpenGL 3.3
            // we also want to trigger the GL3 fallbacks on these chipsets
            // this is expected to be mainly pre-Haswell Intel HD Graphics 4X00 and 5X00.
            // A lot of these chips claim 4.3 or 4.4 support, but don't seem to work.
            // https://code.blender.org/2019/04/supported-gpus-in-blender-2-80/
            // https://docs.blender.org/manual/en/latest/troubleshooting/gpu/windows/intel.html#legacy-intel-hd-4000-5000
            // https://www.intel.com/content/www/us/en/support/articles/000005524/graphics.html
            // this will disable things like reflection probes, HDR, FXAA and SMAA
            LL_INFOS("RenderInit") << "Applying Intel integrated pre-Haswell fallback.  Downgrading feature usage to OpenGL 3.3" << LL_ENDL;
            gGLManager.mGLVersion = llmin(gGLManager.mGLVersion, 3.33f);
            gGLManager.mGLVersionString += " 3.3 fallback";  // for ViewerStats reporting
            // and select GLSL version for OpenGL 3.2
            gGLManager.mGLSLVersionMajor = 3;
            gGLManager.mGLSLVersionMinor = 20;
        }
    }
    if (gGLManager.mIsApple)
    {
        maskFeatures("AppleGPU");
    }
    else
    {
        maskFeatures("NonAppleGPU");
    }
    if (gGLManager.mGLVersion < 3.f)
    {
        maskFeatures("OpenGLPre30");
    }
    if (gGLManager.mNumTextureImageUnits <= 8)
    {
        maskFeatures("TexUnit8orLess");
    }
    if (gGLManager.mNumTextureImageUnits <= 16)
    {
        maskFeatures("TexUnit16orLess");
    }
    if (gGLManager.mVRAM > 512)
    {
        maskFeatures("VRAMGT512");
    }
    if (gGLManager.mVRAM < 2048)
    {
        maskFeatures("VRAMLT2GB");
    }
    if (gGLManager.mGLVersion < 3.99f)
    {
        maskFeatures("GL3");

        // make sure to disable background context activity in GL3 mode
        LLImageGLThread::sEnabledMedia = false;
        LLImageGLThread::sEnabledTextures = false;

        // Make extra sure that vintage mode also gets enabled.
        gSavedSettings.setBOOL("RenderDisableVintageMode", false);
    }
    if (gGLManager.mMaxVaryingVectors <= 16)
    {
        maskFeatures("VaryingVectors16orLess");
    }

    // now mask by gpu string
    // Replaces ' ' with '_' in mGPUString to deal with inability for parser to handle spaces
    std::string gpustr = mGPUString;
    for (std::string::iterator iter = gpustr.begin(); iter != gpustr.end(); ++iter)
    {
        if (*iter == ' ')
        {
            *iter = '_';
        }
    }

    //LL_INFOS() << "Masking features from gpu table match: " << gpustr << LL_ENDL;
    maskFeatures(gpustr);

    if (isSafe())
    {
        maskFeatures("safe");
    }
}

LLSD LLFeatureManager::getRecommendedSettingsMap()
{
    // Create the map and fill it with the hardware recommended settings.
    // It's needed to create an initial Default graphics preset (MAINT-6435).
    // The process is similar to the one LLFeatureManager::applyRecommendedSettings() does.

    LLSD map(LLSD::emptyMap());

    U32 level = llmax(GPU_CLASS_0, llmin(mGPUClass, GPU_CLASS_5));
    LL_INFOS("RenderInit") << "Getting the map of recommended settings for level " << level << LL_ENDL;

    std::string features(isValidGraphicsLevel(level) ? getNameForGraphicsLevel(level) : "Low");

    maskFeatures(features);

    LLControlVariable* ctrl = gSavedSettings.getControl("RenderQualityPerformance"); // include the quality value for correct preset loading
    map["RenderQualityPerformance"]["Value"] = (LLSD::Integer)level;
    map["RenderQualityPerformance"]["Comment"] = ctrl->getComment();;
    map["RenderQualityPerformance"]["Persist"] = 1;
    map["RenderQualityPerformance"]["Type"] = LLControlGroup::typeEnumToString(ctrl->type());



    for (feature_map_t::iterator mIt = mFeatures.begin(); mIt != mFeatures.end(); ++mIt)
    {
        LLControlVariable* ctrl = gSavedSettings.getControl(mIt->first);
        if (ctrl == NULL)
        {
            LL_WARNS("RenderInit") << "AHHH! Control setting " << mIt->first << " does not exist!" << LL_ENDL;
            continue;
        }

        if (ctrl->isType(TYPE_BOOLEAN))
        {
            map[mIt->first]["Value"] = (LLSD::Boolean)getRecommendedValue(mIt->first);
        }
        else if (ctrl->isType(TYPE_S32) || ctrl->isType(TYPE_U32))
        {
            map[mIt->first]["Value"] = (LLSD::Integer)getRecommendedValue(mIt->first);
        }
        else if (ctrl->isType(TYPE_F32))
        {
            map[mIt->first]["Value"] = (LLSD::Real)getRecommendedValue(mIt->first);
        }
        else
        {
            LL_WARNS("RenderInit") << "AHHH! Control variable is not a numeric type!" << LL_ENDL;
            continue;
        }
        map[mIt->first]["Comment"] = ctrl->getComment();;
        map[mIt->first]["Persist"] = 1;
        map[mIt->first]["Type"] = LLControlGroup::typeEnumToString(ctrl->type());
    }

    return map;
}
