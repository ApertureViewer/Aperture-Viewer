/**
 * @file postDeferredNoDoFF.glsl
 *
 * $LicenseInfo:firstyear=2007&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2007, Linden Research, Inc.
 * Copyright (C) 2025, William Weaver (paperwork) @ Second Life
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

/*[EXTRA_CODE_HERE]*/

out vec4 frag_color;

uniform sampler2D diffuseRect;
uniform sampler2D depthMap;

uniform vec2 screen_res;
in vec2 vary_fragcoord; // REMOVED - Replaced with gl_FragCoord

// <AP:WW> START: Aperture Post-Processing Uniforms
uniform float ap_saturation;        // 0=gray, 1=normal, >1=more saturated
uniform vec3  ap_luminance_weights; // User-defined RGB weights for luminance
uniform float ap_contrast;          // -100 to 100
uniform float ap_highlights;        // -100 to 100
uniform float ap_shadows;           // -100 to 100
uniform float ap_whites;            // -100 to 100
uniform float ap_blacks;            // -100 to 100
uniform float ap_temperature;       // -100 to 100 (Cool/Blue to Warm/Yellow)
uniform float ap_tint;              // -100 to 100 (Green to Magenta)
uniform float ap_vibrance;          // -100 to 100
uniform float ap_grain_amount;      // 0 to 100
uniform float ap_grain_size;        // 0 to 100
uniform float ap_grain_roughness;   // 0 to 100
uniform float ap_ca_red_cyan;       // Chromatic Aberration Red/Cyan Shift [-100, 100]
uniform float ap_ca_green_magenta;  // Chromatic Aberration Green/Magenta Shift [-100, 100]
uniform float ap_ca_blue_yellow;    // Chromatic Aberration Blue/Yellow Shift [-100, 100]
uniform float ap_ca_softness;
uniform float ap_crush_black;       // Normalized 0.0 to 1.0 (from C++ 0-255)
uniform float ap_crush_black_fade_end; // Input: 0.0-1.0 from C++
uniform float ap_cb_shads_r;         // Shadows Red/Cyan [-100, 100]
uniform float ap_cb_shads_g;         // Shadows Green/Magenta [-100, 100]
uniform float ap_cb_shads_b;         // Shadows Blue/Yellow [-100, 100]
uniform float ap_cb_mids_r;          // Midtones Red/Cyan [-100, 100]
uniform float ap_cb_mids_g;          // Midtones Green/Magenta [-100, 100]
uniform float ap_cb_mids_b;          // Midtones Blue/Yellow [-100, 100]
uniform float ap_cb_lites_r;         // Highlights Red/Cyan [-100, 100]
uniform float ap_cb_lites_g;         // Highlights Green/Magenta [-100, 100]
uniform float ap_cb_lites_b;         // Highlights Blue/Yellow [-100, 100]
uniform int   ap_cb_preserve_luma;   // Preserve Luminosity (0 or 1)

// <AP:WW> END: Aperture Post-Processing Uniforms


//=================================
// Noise Functions (Unchanged)
//=================================
float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(float x) {
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f);
    return mix(hash(i), hash(i + 1.0), u);
}

float noise(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

//=================================
// Effect Functions (Unchanged)
//=================================

// <AP:WW> START: Sophisticated Tone Mapping Function (Masked H/S/W/B/Contrast)

// Helper function for Luminance Calculation (using Rec.709 standard weights)
float calculateLuminance(vec3 color) {
    return dot(max(color, 0.0), vec3(0.2126, 0.7152, 0.0722));
}

// Helper to safely divide, avoiding division by zero
vec3 safeDivide(vec3 num, float den) {
    return (den == 0.0) ? vec3(0.0) : num / den;
}

vec3 clampHDRRange(vec3 color);

vec3 applyToneMapping(vec3 color)
{
    // --- Tunable: Contrast Control ---
    const float CONTRAST_PIVOT = 0.55;          // Midpoint for contrast adjustment (0-1, typically 0.5)
    const float CONTRAST_BASE = 1.5;           // Base for contrast exponentiation (controls intensity curve shape)
    const float CONTRAST_SENSITIVITY = 350.0;  // Divisor for ap_contrast (controls slider sensitivity)
    // --- Tunable: Contrast Masking ---
    const float CONTRAST_MASK_LOW_END = 0.001;   // Below this luminance, contrast has zero effect
    const float CONTRAST_MASK_LOW_START = 0.175; // Contrast starts fading in above this luminance
    const float CONTRAST_MASK_HIGH_START = 0.75;// Contrast starts fading out above this luminance
    const float CONTRAST_MASK_HIGH_END = 0.999;  // Above this luminance, contrast has zero effect
    // --- Tunable: Saturation Compensation ---
    const float SATURATION_COMP_STRENGTH = 1.0; // How strongly to preserve original saturation (0=none, 1=full attempt)
    const float SATURATION_COMP_AMPLIFY = 1.0; // Multiplier >= 1. Increase to make effect more visible. Tune this!

    // --- Tunable: Highlight Masking Range ---
    const float HIGHLIGHT_MASK_START = 0.4;
    const float HIGHLIGHT_MASK_FULL  = 0.999;
    // --- Tunable: Shadow Masking Range ---
    const float SHADOW_MASK_FULL     = 0.001;
    const float SHADOW_MASK_START    = 0.4;
    // --- Tunable: Black Point Masking Range ---
    const float BLACK_MASK_FULL      = 0.0;   // <<< ADDED
    const float BLACK_MASK_END       = 0.5;
    // --- Tunable: White Point Masking Range ---
    const float WHITE_MASK_START     = 0.75;
    const float WHITE_MASK_FULL      = 1.0;   // <<< ADDED
    // --- Tunable: Black Point Adjustment Strength ---
    const float MAX_BLACK_OFFSET_SCALE = 0.085;
    // --- Tunable: White Point Adjustment Strength ---
    const float MAX_WHITE_OFFSET_SCALE = 0.015;
    // --- Tunable: H/S Curve Strength ---
    const float HS_CURVE_BASE = 1.5;
    // --- Tunable: H/S Slider Sensitivity ---
    const float HS_SENSITIVITY = 100.0;
    // --- Tunable: W/B Slider Sensitivity & Inversion ---
    const float WB_NORMALIZATION_SCALE = 1.0 / 100.0;


    // --- 1. Contrast (Sophisticated: Luminance-Based, Masked, Pivoted) ---
    float contrast_factor = pow(CONTRAST_BASE, ap_contrast / CONTRAST_SENSITIVITY);
    if (abs(contrast_factor - 1.0) > 0.001)
    {
        float lum = calculateLuminance(color);
        float low_fade = smoothstep(CONTRAST_MASK_LOW_END, CONTRAST_MASK_LOW_START, lum);
        float high_fade = 1.0 - smoothstep(CONTRAST_MASK_HIGH_START, CONTRAST_MASK_HIGH_END, lum);
        float contrast_mask = low_fade * high_fade;

        if (contrast_mask > 0.001)
        {
            float adjusted_lum = CONTRAST_PIVOT + (lum - CONTRAST_PIVOT) * contrast_factor;
            float final_lum = mix(lum, adjusted_lum, contrast_mask);

            if (lum > 0.0001) {
                // Calculate the luminance change
                float lum_change = final_lum - lum;

                // Calculate the simple brightness addition vector
                vec3 simple_lum_add = vec3(lum_change);

                // Calculate the ideal saturation-compensated addition vector
                vec3 compensated_add = (color / lum) * lum_change;

                // Calculate the *difference* vector introduced by compensation
                vec3 compensation_difference = compensated_add - simple_lum_add;

                // Amplify this difference vector
                vec3 amplified_difference = compensation_difference * SATURATION_COMP_AMPLIFY;

                // Calculate the final vector to add:
                // Start with the simple brightness change, then add the
                // amplified compensation difference, blended by the strength slider.
                // If STRENGTH = 1.0, we add the full amplified difference.
                // If STRENGTH = 0.0, we add zero difference (effectively just simple_lum_add).
                vec3 final_add_vector = simple_lum_add + (amplified_difference * SATURATION_COMP_STRENGTH);

                // Apply the final calculated change
                color += final_add_vector;

            } else {
                 color = vec3(max(0.0, final_lum));
            }
            color = clamp(color, 0.0, 1.0);
        }
    }


    // --- 2. Highlights (Masked Application - Power Curve) ---
    float h_adjust = ap_highlights / HS_SENSITIVITY; // Use HS sensitivity
    if (abs(h_adjust) > 0.001) {
        float current_luminance = calculateLuminance(color);
        float highlight_mask = smoothstep(HIGHLIGHT_MASK_START, HIGHLIGHT_MASK_FULL, current_luminance);
        if (highlight_mask > 0.001) {
            float highlight_exponent = pow(HS_CURVE_BASE, -h_adjust); // Use HS base
            vec3 highlight_adjusted_color = pow(color, vec3(highlight_exponent));
            color = mix(color, highlight_adjusted_color, highlight_mask);
        }
    }

    // --- 3. Shadows (Masked Application - Power Curve) ---
    float s_adjust = ap_shadows / HS_SENSITIVITY; // Use HS sensitivity
    if (abs(s_adjust) > 0.001) {
        float current_luminance = calculateLuminance(color);
        float shadow_mask = 1.0 - smoothstep(SHADOW_MASK_FULL, SHADOW_MASK_START, current_luminance);
        if (shadow_mask > 0.001) {
            float shadow_exponent = pow(HS_CURVE_BASE, s_adjust); // Use HS base
            vec3 shadow_adjusted_color = 1.0 - pow(1.0 - color, vec3(shadow_exponent));
            color = mix(color, shadow_adjusted_color, shadow_mask);
        }
    }

    // Clamp after H/S adjustments before W/B
    color = clamp(color, 0.0, 1.0);

    // --- 4. Blacks (Masked Application - Offset) ---
    // We invert the slider effect: positive crushes (negative offset), negative lifts (positive offset)
    float b_adjust_norm = ap_blacks * WB_NORMALIZATION_SCALE; // Use WB normalization scale
    if (abs(b_adjust_norm) > 0.001) {
        float current_luminance = calculateLuminance(color);
        float black_mask = 1.0 - smoothstep(BLACK_MASK_FULL, BLACK_MASK_END, current_luminance); // Mask uses implicit FULL=0.0
        if (black_mask > 0.001) {
            float black_offset = b_adjust_norm * MAX_BLACK_OFFSET_SCALE;
            color += vec3(black_offset * black_mask);
        }
    }

    // --- 5. Whites (Masked Application - Offset) ---
    // Corrected: Positive slider pushes (positive offset), negative pulls (negative offset)
    float w_adjust_norm = ap_whites * WB_NORMALIZATION_SCALE; // REMOVED the '-' sign before ap_whites
    if (abs(w_adjust_norm) > 0.001) {
        float current_luminance = calculateLuminance(color);
        float white_mask = smoothstep(WHITE_MASK_START, WHITE_MASK_FULL, current_luminance);
        if (white_mask > 0.001) {
            // Calculate offset: positive slider pushes (positive offset), negative pulls (negative offset)
            float white_offset = w_adjust_norm * MAX_WHITE_OFFSET_SCALE;
            color += vec3(white_offset * white_mask);
        }
    }

    // --- 6. Final Clamp ---
    color = clamp(color, 0.0, 1.0);

    return color; // Return the fully adjusted color
}
// <AP:WW> END: Sophisticated Tone Mapping Function

// Modify the applyBlackCrush function:
// <AP:WW> Add Black Crush Function Definition (Post-ToneMap Version - Handles 0-255 input, uses fade_end uniform)
vec3 applyBlackCrush(vec3 color)
{
    // 1. Check if crush amount is significant (using 0-255 scale)
    if (ap_crush_black <= 0.1) {
        return color;
    }

    // 1b. Normalize the incoming 0-255 crush value to the 0.0-1.0 range
    float normalized_crush = ap_crush_black / 255.0;

    // 2. Define the Luminance Range for the Effect Fade:
    const float LIFT_FADE_START = 0.0;
    //    Use the uniform for the fade end point.
    float lift_fade_end = ap_crush_black_fade_end; // Read from uniform

    //    *** SAFETY CHECK ***: Ensure fade_end is always slightly larger than fade_start
    //    to prevent issues with smoothstep if they become equal or inverted.
    lift_fade_end = max(lift_fade_end, LIFT_FADE_START + 0.001);

    // 3. Define Standard Luminance Weights: (Standard)
    const vec3 lum_weights_std = vec3(0.2126, 0.7152, 0.0722);

    // 4. Calculate the Luminance of the Input Color: (Uses 0.0-1.0 range)
    float lum = dot(max(color.rgb, vec3(0.0)), lum_weights_std);

    // 5. Calculate the Blend Factor based on Luminance: (Use the variable lift_fade_end)
    float blend_factor = smoothstep(lift_fade_end, LIFT_FADE_START, lum); // Use variable here

    // 6. Determine the Maximum Lift Amount: (Use the NORMALIZED value)
    vec3 lift_amount = vec3(normalized_crush);

    // 7. Apply the Lift: (Standard color math)
    color = color + lift_amount * blend_factor;

    // 8. Final Clamp and Return: (Standard)
    return clamp(color, 0.0, 1.0);
}
// </AP:WW>

// <AP:WW> START: Color Grading Functions (Unchanged from working version)
vec3 applyTempTint(vec3 color)
{
    float temp_factor = ap_temperature / 1000.0;
    float tint_factor = ap_tint / 1000.0;
    color.r += temp_factor;
    color.b -= temp_factor;
    color.g += tint_factor;
    return color;
}

vec3 applyVibrance(vec3 color)
{
    float vib = ap_vibrance / 100.0;
    float max_comp = max(color.r, max(color.g, color.b));
    float min_comp = min(color.r, min(color.g, color.b));
    float sat = max_comp - min_comp;
    const vec3 lum_weights = vec3(0.299, 0.587, 0.114);
    float lum = dot(color, lum_weights);
    vec3 gray = vec3(lum);
    float sat_factor = pow(1.0 - sat, 2.0);
    if (vib > 0.0) {
        color = mix(gray, color, 1.0 + vib * sat_factor);
    } else {
        color = mix(gray, color, 1.0 + vib);
    }
    return color;
}
// <AP:WW> END: Color Grading Functions

// <AP:WW> START: Chromatic Aberration Function (Shifts Original Image)
vec3 applyChromaticAberration(vec2 uv, sampler2D originalTex)
{
    if (abs(ap_ca_red_cyan) < 0.01 && abs(ap_ca_green_magenta) < 0.01 && abs(ap_ca_blue_yellow) < 0.01) {
        return texture(originalTex, uv).rgb;
    }
    vec2 offset = uv - vec2(0.5);
    const float scale = 0.0003; // Tune this
    vec2 redOffset   = offset * ap_ca_red_cyan * scale;
    vec2 greenOffset = offset * ap_ca_green_magenta * scale;
    vec2 blueOffset  = offset * ap_ca_blue_yellow * scale;
    float r = texture(originalTex, uv + redOffset).r;
    float g = texture(originalTex, uv + greenOffset).g;
    float b = texture(originalTex, uv + blueOffset).b;
    return vec3(r, g, b);
}
// <AP:WW> END: Chromatic Aberration Function (Shifts Original Image)

// <AP:WW> START: Grain Function (Coordinate Warping)
vec3 applyGrain(vec3 color, vec2 uv_in) 
{
    float amount = ap_grain_amount / 100.0; // Overall strength (0-1)
    if (amount <= 0.0) return color;

    // --- 1. Base Coordinate ---
    // Use screen coordinates. We might scale them slightly by a fixed amount
    // if needed, but NOT by ap_grain_size here.
    vec2 base_uv = gl_FragCoord.xy * .5; // Example fixed scale - tune if needed

    // --- 2. Calculate Coordinate Warp Offset ---
    // Use a hash/noise function to get a pseudo-random offset vector.
    // The magnitude of this offset is controlled by ap_grain_size.
    float size_scale = ap_grain_size / 100.0; // Map size [0,100] -> [0,1]
    // *** Tune MAX_WARP_STRENGTH to control how much size affects distortion ***
    float MAX_WARP_STRENGTH = 2.5; // Max offset in "noise units" at size=100
    vec2 warp_offset = vec2(hash(base_uv) - 0.5, hash(base_uv + vec2(5.0, 3.0)) - 0.5); // Get two offset values [-0.5, 0.5]
    warp_offset *= size_scale * MAX_WARP_STRENGTH; // Scale offset by size parameter

    // Apply the warp to the base coordinate
    vec2 warped_uv = base_uv + warp_offset;

    // --- 3. Calculate Base Noise Value using Warped Coordinate ---
    // Use a different hash/noise function with the distorted coordinate.
    float base_noise_val = hash(warped_uv + vec2(11.0, 17.0)) - 0.5; // Noise [-0.5, 0.5]

    // --- 4. Apply Roughness Modulation (Optional Intensity Variation) ---
    // Roughness can still add local intensity variation on top of the spatial warp.
    // This uses the ORIGINAL base_uv to ensure variation is local and not stretched by the warp.
    if (ap_grain_roughness > 0.0)
    {
        float roughness_random = hash(base_uv + vec2(23.0, 29.0)); // Yet another hash
        // *** Tune roughness_scale for desired intensity variation ***
        float roughness_scale = 50;
        float roughness_mod = 1.0 + (roughness_random - 0.5) * roughness_scale * (ap_grain_roughness / 100.0);
        roughness_mod = max(0.0, roughness_mod);
        base_noise_val *= roughness_mod; // Modulate the noise intensity
    }

    // --- 5. Calculate Per-Pixel Luminance Modulation Factor ---
    // Fades grain in deep shadows and bright highlights.
    const vec3 lum_weights = vec3(0.2126, 0.7152, 0.0722);
    float lum = dot(max(color, 0.0), lum_weights);
    // *** Tune fade thresholds (0.1, 0.8) if needed ***
    float lum_factor = smoothstep(0.0, 0.2, lum) * (1.0 - smoothstep(0.8, 1.0, lum));
    lum_factor = clamp(lum_factor, 0.0, 1.0);

    // --- 6. Calculate Final Intensity Scaling ---
    // Controls the overall strength of the grain.
    // *** Tune base_intensity_scale (e.g., 0.1 to 0.2) ***
    float base_intensity_scale = 0.15;
    float final_intensity_scale = amount * base_intensity_scale * lum_factor;

    // --- 7. Apply final noise value ---
    color += vec3(base_noise_val * final_intensity_scale);

    return color; // Rely on final clamp in main
}
// <AP:WW> END: Grain Function (Coordinate Warping)

// *** Ensure you have a vec2 version of hash if needed ***
// Example if hash only takes float:
// float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }
// vec2 hash2D(vec2 p) { // Or similar name
//     return vec2(hash(p), hash(p + vec2(7.0, 23.0))); // Example using offset
// }
// Then replace hash(vec2_input) with appropriate calls like hash(vec2_input.x + vec2_input.y) or use hash2D etc.
// The provided code assumes your hash() function already handles vec2 input appropriately.

    

// <AP:WW> START: Weighted 5x5 Gaussian Chromatic Aberration Helper
// Calculates the CA effect for 25 points (center + neighbors in a 5x5 grid)
// and applies Gaussian weights for a smoother, higher-quality blur.
// NOTE: VERY EXPENSIVE - calls applyChromaticAberration 25 times.
vec3 getBlurredChromaticAberration(vec2 uv, sampler2D originalTex)
{
    vec2 pixel_size = 1.0 / screen_res; // Size of one pixel in UV coordinates

    // Use the distance multiplier you found effective (e.g., 10.0)
    const float BLUR_DISTANCE_MULTIPLIER = 10.0; // **** Tweak this distance ****

    vec3 blurred_color = vec3(0.0);
    float weight_sum = 0.0;

    // 5x5 kernel offsets (from -2 to +2)
    vec2 offsets[25] = vec2[](
        vec2(-2.0, -2.0), vec2(-1.0, -2.0), vec2( 0.0, -2.0), vec2( 1.0, -2.0), vec2( 2.0, -2.0),
        vec2(-2.0, -1.0), vec2(-1.0, -1.0), vec2( 0.0, -1.0), vec2( 1.0, -1.0), vec2( 2.0, -1.0),
        vec2(-2.0,  0.0), vec2(-1.0,  0.0), vec2( 0.0,  0.0), vec2( 1.0,  0.0), vec2( 2.0,  0.0),
        vec2(-2.0,  1.0), vec2(-1.0,  1.0), vec2( 0.0,  1.0), vec2( 1.0,  1.0), vec2( 2.0,  1.0),
        vec2(-2.0,  2.0), vec2(-1.0,  2.0), vec2( 0.0,  2.0), vec2( 1.0,  2.0), vec2( 2.0,  2.0)
    );

    // Corresponding 5x5 Gaussian weights (approximated from 1D kernel {1, 4, 6, 4, 1})
    // Total weight = (1+4+6+4+1)^2 = 16^2 = 256
    float weights[25] = float[](
         1.0,  4.0,  6.0,  4.0,  1.0,
         4.0, 16.0, 24.0, 16.0,  4.0,
         6.0, 24.0, 36.0, 24.0,  6.0,
         4.0, 16.0, 24.0, 16.0,  4.0,
         1.0,  4.0,  6.0,  4.0,  1.0
    );


    for (int i = 0; i < 25; i++) // Loop over all 25 samples
    {
        // Calculate UV for the neighbor, using the distance multiplier
        vec2 sample_uv = uv + offsets[i] * pixel_size * BLUR_DISTANCE_MULTIPLIER;

        // Get the weight for this sample
        float current_weight = weights[i];

        // Calculate the CA effect at that neighbor's position and apply weight
        blurred_color += applyChromaticAberration(sample_uv, originalTex) * current_weight;

        // Add the weight to the sum
        weight_sum += current_weight;
    }

    // Average the results using the total weight
    // Check weight_sum defensively, though it should be 256.0
    if (weight_sum > 0.0) {
        return blurred_color / weight_sum;
    } else {
        // Fallback
        return applyChromaticAberration(uv, originalTex); // Fallback to sharp
    }
}
// <AP:WW> END: Weighted 5x5 Gaussian Chromatic Aberration Helper

      
// <AP:WW> START: Color Balance Function
vec3 applyColorBalance(vec3 color)
{
    // --- Tunables ---
    const float SLIDER_SCALE = 1.0 / 500.0; // Scale C++ slider range [-100, 100] to [-1, 1]
    const float LIFT_GAMMA_FALLOFF = 1.5;   // Controls Shadow mask falloff (higher = sharper)
    const float GAIN_GAMMA_FALLOFF = 1.5;   // Controls Highlight mask falloff (higher = sharper)
    const float MID_GRAY_POINT = 0.5;       // Center point for midtone influence
    const float LUM_PRESERVE_STRENGTH = 1.0; // Strength of luminance preservation (0=off, 1=full)

    // --- Luminance Calculation (Standard Rec.709 for masking) ---
    const vec3 lum_weights_std = vec3(0.2126, 0.7152, 0.0722);
    float luminance = dot(max(color, 0.0), lum_weights_std);

    // --- Calculate Masks (Lift/Shadows, Gamma/Midtones, Gain/Highlights) ---
    // Shadow Mask ("Lift" in DaVinci terms) - strong in shadows, fades out
    float shadow_mask = pow(1.0 - luminance, LIFT_GAMMA_FALLOFF);

    // Highlight Mask ("Gain") - strong in highlights, fades out
    float highlight_mask = pow(luminance, GAIN_GAMMA_FALLOFF);

    // Midtone Mask ("Gamma") - Peaks around mid-gray, fades towards shadows/highlights
    // Ensure masks don't overlap excessively and sum roughly to 1 near the crossover points.
    // We use a more robust method than simple 1-s-h to avoid negative values if falloffs are steep.
    float midtone_mask = 1.0 - shadow_mask - highlight_mask;
    midtone_mask = max(0.0, midtone_mask); // Clamp to prevent negative values due to pow() functions

    // Normalize masks slightly to ensure they sum closer to 1.0 where they overlap
    float total_mask = shadow_mask + midtone_mask + highlight_mask;
    if (total_mask > 0.001) {
        shadow_mask /= total_mask;
        midtone_mask /= total_mask;
        highlight_mask /= total_mask;
    }

    // --- Normalize Input Settings and Create Offset Vectors ---
    vec3 offset_shads = vec3(ap_cb_shads_r, ap_cb_shads_g, ap_cb_shads_b) * SLIDER_SCALE;
    vec3 offset_mids  = vec3(ap_cb_mids_r,  ap_cb_mids_g,  ap_cb_mids_b)  * SLIDER_SCALE;
    vec3 offset_lites = vec3(ap_cb_lites_r, ap_cb_lites_g, ap_cb_lites_b) * SLIDER_SCALE;

    // --- Apply Masked Offsets ---
    // Add the offsets weighted by their respective masks
    vec3 adjusted_color = color +
                          offset_shads * shadow_mask +
                          offset_mids  * midtone_mask +
                          offset_lites * highlight_mask;

    // --- Preserve Luminance (Optional) ---
    if (ap_cb_preserve_luma == 1 && LUM_PRESERVE_STRENGTH > 0.0)
    {
        float lum_adjusted = dot(max(adjusted_color, 0.0), lum_weights_std);
        float lum_diff = luminance - lum_adjusted; // Original Luma - Adjusted Luma

        // Add the luminance difference back proportionally
        // Avoid division by zero if adjusted luminance is black
        if (lum_adjusted > 0.0001) {
             // Simple method: Add difference directly
             // adjusted_color += vec3(lum_diff * LUM_PRESERVE_STRENGTH);

             // More stable method: Scale color to match original luminance
             adjusted_color = adjusted_color * (luminance / lum_adjusted);
             // Blend based on strength if needed (though 1.0 is common)
             adjusted_color = mix(color + offset_shads * shadow_mask + offset_mids * midtone_mask + offset_lites * highlight_mask, // Re-calculate non-preserved for mix base
                                  adjusted_color,
                                  LUM_PRESERVE_STRENGTH);

        } else if (luminance > 0.0) {
            // If adjusted is black but original wasn't, just add back the original luminance as gray
             adjusted_color = vec3(luminance * LUM_PRESERVE_STRENGTH);
        }
         // If both are black, do nothing
    }

    // --- Final Clamp and Return ---
    // Clamp is essential after adding offsets
    return clamp(adjusted_color, 0.0, 1.0);
}
// <AP:WW> END: Color Balance Function

    

void main()
{
    // --- Initial Setup ---
    // We use vary_fragcoord passed from the vertex shader, assuming it's correct.
    vec4 original_sample = texture(diffuseRect, vary_fragcoord.xy);
    vec3 processed_color = original_sample.rgb; // Start with original color

// --- Main Image Processing Pipeline ---
#ifdef HAS_NOISE
    // --- NOISE PATH for Base ---
    // Note: Using vary_fragcoord * screen_res might be better for noise consistency if needed.
    // Keeping original noise calculation for now. Re-evaluate if noise looks wrong in snapshots.
    vec2 tc = vary_fragcoord.xy * screen_res * 4.0; // Or gl_FragCoord.xy * 4.0; ? Test if needed.
    vec3 seed = (processed_color + vec3(1.0)) * vec3(tc.xy, tc.x + tc.y);
    vec3 nz = vec3(noise(seed.rg), noise(seed.gb), noise(seed.rb));
    processed_color += nz * 0.003;
    processed_color = clamp(processed_color, 0.0, 1.0);

    // Apply effects
    processed_color = applyToneMapping(processed_color);
    processed_color = applyBlackCrush(processed_color);
    processed_color = applyTempTint(processed_color);
    vec3 weights_n = ap_luminance_weights / max(dot(ap_luminance_weights, vec3(1.0)), 0.0001);
    float luminance_n = dot(processed_color, weights_n);
    vec3 gray_version_n = vec3(luminance_n);
    processed_color = mix(gray_version_n, processed_color, ap_saturation);
    processed_color = applyVibrance(processed_color);
    processed_color = applyColorBalance(processed_color);

#else // Start of NON-NOISE PATH for Base
    // --- NON-NOISE PATH for Base ---

    // Apply effects
    processed_color = applyToneMapping(processed_color);
    processed_color = applyBlackCrush(processed_color);
    processed_color = applyTempTint(processed_color);
    vec3 weights_nn = ap_luminance_weights / max(dot(ap_luminance_weights, vec3(1.0)), 0.0001);
    float luminance_nn = dot(processed_color, weights_nn);
    vec3 gray_version_nn = vec3(luminance_nn);
    processed_color = mix(gray_version_nn, processed_color, ap_saturation);
    processed_color = applyVibrance(processed_color);
    processed_color = applyColorBalance(processed_color);

#endif // End HAS_NOISE / NON-NOISE PATH

    vec3 base_processed_color = processed_color; // Color before CA and Grain

    // --- Chromatic Aberration with Softness (Blurring Approach) --- //
    // Calculate the sharp CA result first (Using vary_fragcoord)
    vec3 sharp_ca_color = applyChromaticAberration(vary_fragcoord.xy, diffuseRect);

    // Calculate the final CA color, blending towards a blurred version based on softness
    vec3 final_ca_color;
    if (ap_ca_softness <= 0.01) // Treat near-zero as fully sharp (optimization)
    {
        final_ca_color = sharp_ca_color;
    }
    else
    {
        // Calculate the blurred CA result (EXPENSIVE) (Using vary_fragcoord)
        vec3 blurred_ca_color = getBlurredChromaticAberration(vary_fragcoord.xy, diffuseRect);
        // Blend between sharp and blurred based on softness
        final_ca_color = mix(sharp_ca_color, blurred_ca_color, ap_ca_softness);
    }

    // Calculate the difference using the (potentially blurred) final CA color
    vec3 ca_difference = final_ca_color - original_sample.rgb;
    float fringe_magnitude = length(ca_difference);

    // Use the ORIGINAL fixed thresholds for the blend factor now,
    // because the softening is part of final_ca_color itself.
    const float threshold_low = 0.02;
    const float threshold_high = 0.2;
    // Define fringe_blend_factor *before* the if statement
    float fringe_blend_factor = smoothstep(threshold_low, threshold_high, fringe_magnitude);

    // --- Luminance-Matching Blend ---
    // Initialize final_color_pre_grain *before* the if statement
    vec3 final_color_pre_grain = base_processed_color; // Default to base color

    if (fringe_blend_factor > 0.0)
    {
        const vec3 lum_weights_std = vec3(0.2126, 0.7152, 0.0722);
        float base_lum = dot(base_processed_color, lum_weights_std);

        // Luminance match using the *potentially blurred* final_ca_color
        float ca_affected_lum = dot(final_ca_color, lum_weights_std);
        vec3 ca_lum_matched; // Define here, inside if needed
        if (ca_affected_lum < 0.0001) {
            ca_lum_matched = vec3(base_lum);
        } else {
            ca_lum_matched = final_ca_color * (base_lum / ca_affected_lum);
        }
        // Mix using the calculated fringe_blend_factor
        // Assign the result back to final_color_pre_grain
        final_color_pre_grain = mix(base_processed_color, ca_lum_matched, fringe_blend_factor);
    }
    // Assign the potentially blended color back
    processed_color = final_color_pre_grain;

    // --- Apply Grain Last ---
    // Pass vary_fragcoord.xy to the grain function
    processed_color = applyGrain(processed_color, vary_fragcoord.xy);

    // --- Final Output ---
    processed_color = clamp(processed_color, 0.0, 1.0);
    frag_color.rgb = processed_color;
    frag_color.a = original_sample.a;
    gl_FragDepth = texture(depthMap, vary_fragcoord.xy).r;
}