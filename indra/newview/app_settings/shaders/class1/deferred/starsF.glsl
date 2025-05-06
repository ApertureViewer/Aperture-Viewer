/**
 * @file starsF.glsl
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

out vec4 frag_data[4];

// Input varyings from Vertex Shader
in vec4 vertex_color;     // Base vertex color (usually white for stars)
in vec2 vary_texcoord0;   // Texture coordinates for diffuseMap
in vec2 screenpos;        // Stable position derived from vertex position (for noise)
in float vary_intensity; // Per-star intensity (0.0 to 1.0 range)
in vec3 vary_worldDir;    // World direction vector for horizon dimming

// Uniforms from C++
uniform sampler2D diffuseMap; // Star texture (e.g., soft dot)
uniform float custom_alpha;   // Global alpha factor (Star Brightness setting, 0.0 to 1.0)
uniform float time;           // Time for animation (linearly increasing, wrapped in C++)

// --- Procedural Noise Functions (Unchanged) ---
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 mod289(vec4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 permute(vec4 x) { return mod289(((x*34.0)+10.0)*x); }
vec4 taylorInvSqrt(vec4 r) { return 1.79284291400159 - 0.85373472095314 * r; }

vec3 fade(vec3 t) { return t*t*t*(t*(t*6.0-15.0)+10.0); }
vec2 fade(vec2 t) { return t*t*t*(t*(t*6.0-15.0)+10.0); }

float grad_noise(vec2 P)
{
    vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
    vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
    Pi = mod289(Pi);
    vec4 ix = Pi.xzxz;
    vec4 iy = Pi.yyww;
    vec4 fx = Pf.xzxz;
    vec4 fy = Pf.yyww;
    vec4 i = permute(permute(ix) + iy);
    vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
    vec4 gy = abs(gx) - 0.5 ;
    vec4 tx = floor(gx + 0.5);
    gx = gx - tx;
    vec2 g00 = vec2(gx.x,gy.x);
    vec2 g10 = vec2(gx.y,gy.y);
    vec2 g01 = vec2(gx.z,gy.z);
    vec2 g11 = vec2(gx.w,gy.w);
    vec4 norm = taylorInvSqrt(vec4(dot(g00,g00), dot(g01,g01), dot(g10,g10), dot(g11,g11)));
    g00 *= norm.x;
    g01 *= norm.y;
    g10 *= norm.z;
    g11 *= norm.w;
    float n00 = dot(g00, vec2(fx.x, fy.x));
    float n10 = dot(g10, vec2(fx.y, fy.y));
    float n01 = dot(g01, vec2(fx.z, fy.z));
    float n11 = dot(g11, vec2(fx.w, fy.w));
    vec2 fade_xy = fade(Pf.xy);
    vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
    float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
    return 2.3 * n_xy; // Scale output
}
// --- End Noise Functions ---

// --- START TWINKLE MODIFICATION ---
// Calculates atmospheric twinkle including color scintillation.
// Returns vec4: .rgb = additive color shift, .a = multiplicative alpha factor
vec4 atmospheric_twinkle(vec2 stable_pos, float intensity, float time, float worldDirZ) {
    // --- Base Noise Calculation ---
    // Tunable: Noise spatial scale and animation speed
    float noise_scale = 50.0;
    float time_scale = 0.3;
    vec2 noise_coord = stable_pos * noise_scale;
    // Use different time offsets for R, G, B noise lookups for color variation
    float noise_r = grad_noise(noise_coord + vec2(time * time_scale, 0.0));
    float noise_g = grad_noise(noise_coord + vec2(time * time_scale * 1.1, 0.1)); // Slightly different speed/offset
    float noise_b = grad_noise(noise_coord + vec2(time * time_scale * 0.9, 0.2)); // Slightly different speed/offset

    // --- Altitude / Airmass Effect ---
    // Approximate airmass: increases significantly near horizon (Z=0)
    // Tunable: Controls how strongly airmass increases twinkle amplitude
    float airmass_effect_strength = 2.5;
    // Calculate factor that increases from 1 at zenith (Z=1) towards horizon (Z=0)
    // Using smoothstep avoids extremely high values right at Z=0 if 1/Z was used.
    float airmass_factor = 1.0 + smoothstep(0.3, 0.0, worldDirZ) * airmass_effect_strength; // Stronger effect below Z=0.3

    // --- Intensity Effect ---
    // Calculate base amplitude based on intensity (brighter = more potential twinkle)
    // Tunable: Intensity thresholds and max base amplitude multiplier
    float base_amplitude_alpha = 0.3;  // Base amplitude for brightness variation
    float base_amplitude_color = 0.04; // Base amplitude for color shift (MUCH smaller!)
    float intensity_factor_amp = smoothstep(0.1, 0.7, intensity); // How much intensity affects amp

    // --- Combine Effects for Final Amplitudes ---
    // Final amplitude is base amplitude * intensity effect * airmass effect
    float final_amplitude_alpha = base_amplitude_alpha * intensity_factor_amp * airmass_factor;
    float final_amplitude_color = base_amplitude_color * intensity_factor_amp * airmass_factor;

    // --- Calculate Final Factors ---
    // Brightness factor (centered around 1.0) - use average noise or just one channel? Green is common.
    float twinkle_factor_alpha = 1.0 + (noise_g * final_amplitude_alpha);
    twinkle_factor_alpha = clamp(twinkle_factor_alpha, 0.1, 2.0); // Safety clamp alpha

    // Additive color shift (centered around 0.0)
    vec3 twinkle_color_shift = vec3(
        noise_r * final_amplitude_color,
        noise_g * final_amplitude_color,
        noise_b * final_amplitude_color
    );
    // Safety clamp color shift (optional, but can prevent extreme hues)
    twinkle_color_shift = clamp(twinkle_color_shift, -0.2, 0.2);

    return vec4(twinkle_color_shift, twinkle_factor_alpha);
}
// --- END TWINKLE MODIFICATION ---


// --- Main Shader Function ---
void main()
{
    // 1. Base Texture Color & Alpha
    vec4 tex_color = texture(diffuseMap, vary_texcoord0.xy);
    vec3 base_rgb = tex_color.rgb * vertex_color.rgb;
    float texture_alpha = tex_color.a;

    // 2. Intensity Factor (from C++)
    float intensity_factor = clamp(vary_intensity, 0.0, 1.0);
    // Optional: intensity_factor = pow(intensity_factor, 0.75); // Apply curve?

    // 3. Global Alpha Factor (from C++)
    float global_alpha_factor = smoothstep(0.0, 0.9, custom_alpha); // Tune 0.9?

    // 4. Horizon Atmospheric Extinction (Color Dependent) (Unchanged from previous step)
    float worldDirZ = vary_worldDir.z; // Altitude proxy: 1=zenith, 0=horizon
    const vec3 SCATTERING_COEFFS = vec3(0.17, 0.45, 1.0); // R, G, B relative scattering
    const float EXTINCTION_STRENGTH = 3.0; // Tunable (Adjust back from wild values if needed)
    const float HORIZON_FADE_START_Z = 0.20; // Tunable
    const float MIN_TRANSMITTANCE = 0.05; // Tunable
    float altitude_factor = 1.0 - smoothstep(0.0, HORIZON_FADE_START_Z, worldDirZ);
    float optical_depth = altitude_factor * EXTINCTION_STRENGTH;
    vec3 transmittance = exp(-optical_depth * SCATTERING_COEFFS);
    transmittance = max(transmittance, vec3(MIN_TRANSMITTANCE));
    float horizon_luminance_factor = transmittance.g;

    // --- START TWINKLE APPLICATION MODIFICATION ---
    // 5. Twinkle Factor & Color Shift (Calculated using NEW function)
    vec4 twinkle_result = atmospheric_twinkle(screenpos, intensity_factor, time, worldDirZ);
    vec3 twinkle_color_shift = twinkle_result.rgb;  // Additive color shift component
    float twinkle_alpha_factor = twinkle_result.a; // Multiplicative alpha factor component
    // Alpha factor is already clamped inside the function
    // --- END TWINKLE APPLICATION MODIFICATION ---

    // 6. Combine Factors for Final Alpha
    float final_alpha = texture_alpha            // Base shape
                      * intensity_factor         // Intrinsic brightness
                      * global_alpha_factor      // Overall brightness slider
                      * horizon_luminance_factor // Horizon dimming (luminance part)
                      * twinkle_alpha_factor;    // Twinkle animation (alpha part)
    final_alpha = clamp(final_alpha, 0.0, 1.0);

    // 7. Final RGB Color
    vec3 final_rgb = base_rgb * transmittance; // Apply atmospheric color filtering
    // Apply additive twinkle color shift *after* atmospheric filtering
    final_rgb = final_rgb + twinkle_color_shift;
    final_rgb = clamp(final_rgb, 0.0, 1.0); // Clamp final color

    // 8. Final Output Color (Base)
    vec4 final_color = vec4(final_rgb, final_alpha);


    // --- G-Buffer Outputs ---
    frag_data[1] = vec4(0.0f); // Typically velocity or unused
    frag_data[2] = vec4(0.0, 1.0, 0.0, GBUFFER_FLAG_SKIP_ATMOS); // Normal Z + flags

// --- REVISED BLOOM CONTRIBUTION LOGIC ---
#if defined(HAS_EMISSIVE)
    // Emissive Path: Write base color to Emissive, add boost for bright stars.
    frag_data[0] = vec4(0.0); // Albedo is black for unlit/emissive stars

    // --- Tunable Bloom Parameters ---
    float bloom_intensity_threshold_low = 0.6;
    float bloom_intensity_threshold_high = 0.9;
    // How much *extra* brightness to add for fully blooming stars (>= 1.0)
    // 1.0 = no boost, 2.0 = double brightness for bloom pass etc.
    float bloom_boost_factor = 1.5;
    // --- End Tunable ---

    // Calculate how much this star should bloom (0.0 to 1.0)
    float bloom_factor = smoothstep(bloom_intensity_threshold_low,
                                     bloom_intensity_threshold_high,
                                     intensity_factor);

    // Calculate the RGB color to write to the emissive buffer.
    // Start with the final calculated RGB.
    vec3 emissive_rgb = final_color.rgb;

    // Add the boosted component *only* for stars that should bloom.
    // The boost scales with the bloom_factor and the boost_factor.
    // (We subtract 1.0 from boost_factor because we only want to add the *extra* part)
    if (bloom_boost_factor > 1.0) {
         emissive_rgb += final_color.rgb * bloom_factor * (bloom_boost_factor - 1.0);
    }
    // Ensure the boost doesn't make color negative if base color was near zero.
    emissive_rgb = max(emissive_rgb, vec3(0.0));

    // Write the potentially boosted RGB and the original alpha to emissive buffer.
    frag_data[3] = vec4(emissive_rgb, final_color.a);

#else
    // Standard Output (No Emissive Buffer)
    // Write final calculated color to the main color buffer (frag_data[0])
    frag_data[0] = final_color;
    // frag_data[3] might be unused or for other data if HAS_EMISSIVE is not defined
#endif
// --- END REVISED BLOOM CONTRIBUTION LOGIC ---

} // End main()