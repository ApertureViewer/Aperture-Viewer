<p align="left">
  <img align="left" width="150" height="150" src="https://github.com/williamweaver/Aperture-Viewer/blob/dev/doc/Aperture_256.png" alt="Aperture Viewer - Advanced Photography and Videomaking Viewer">
</p>

# Aperture Viewer: Nothing Is Not Possible

**Version 1.0.0 | Officially Listed Third-Party Viewer | Pushing Visual Boundaries in Second Life & OpenSim**

<p align="left">
  <a href="https://wiki.secondlife.com/wiki/Third_Party_Viewer_Directory/Aperture_Viewer" target="_blank">
    <img src="https://img.shields.io/badge/Second_Life-Official_TPV-blue?style=for-the-badge&logo=secondlife" alt="Official Second Life Third-Party Viewer">
  </a>
</p>

Aperture Viewer is an advanced, **officially listed Second Life Third-Party Viewer**, open-source and designed for anyone seeking the highest level of visual fidelity, performance, and artistic control in Second Life and OpenSim. Meticulously crafted by William Weaver with tools prized by virtual photographers and videomakers, its enhancements benefit all users who appreciate stunning graphics and streamlined creative workflows. Built upon the robust Firestorm codebase but offering a distinct, refined experience focused on visual excellence, Aperture 1.0.0 delivers the foundational feature set envisioned for the viewer.

---

## Our Guiding Principles & Community Spirit

Aperture Viewer is more than just software; it's a commitment to artistic possibility and a positive community experience within virtual worlds.

### Our Ethos

> **Nothing Is Not Possible.**
> We believe in relentlessly pushing the boundaries of visual creation within virtual worlds. We empower artists by removing limitations, fostering innovation, and providing the tools to realize any creative vision, no matter how ambitious.

This ethos is the fundamental spirit driving Aperture Viewer, reflecting our dedication to overcoming technical hurdles and enabling creators to achieve visuals previously thought unattainable.

### Our Community Values

Beyond the technical goals, Aperture Viewer is rooted in a deep belief in the power of community, kindness, and shared growth.

> We believe in leading with an open ear and a genuine desire to be gentle and understanding. We recognize that for many, life's journey can be challenging, sometimes lonely, and even painful. This project finds its home in virtual worlds like Second Life – platforms that, at their best, offer a space for hope, warmth, connection, and positivity shared between us all.
>
> Through Aperture Viewer, our interactions on GitHub, and within our Discord community, we strive to embody these values. We aim to be kind, to teach what we know, to learn from others, and to grow together.
>
> So, whether you choose to use our work or simply observe from afar, please know that we are trying, in our own way, to contribute positively to this shared virtual experience. We invite you to join us in fostering this spirit.


### Our Mission

> To empower virtual photographers and videomakers by providing the most advanced, intuitive, and powerful open-source viewer, focused on delivering unparalleled visual fidelity, artistic control, and a streamlined creative workflow for Second Life and OpenSim.

Our day-to-day purpose is to serve the creative community by building and refining the best possible toolset for visual artistry in these platforms.

### Our Vision

> To be the indispensable, benchmark viewer for visual creators in virtual worlds, continuously redefining the standards of graphical excellence, performance, and artistic capability, making Aperture Viewer the premier platform where "Nothing Is Not Possible."

We aspire for Aperture Viewer to become the definitive standard, recognized for its leadership in visual quality and creative empowerment.

---

By integrating **a revolutionary, unified Phototools system featuring deep environment editing and a groundbreaking real-time Post-Processing pipeline**, **a completely rewritten procedural Starfield**, performance-tuned graphics presets, and numerous creator-focused enhancements, Aperture Viewer provides a powerful platform to elevate your virtual world experience, embodying its core ethos.

See the visual quality Aperture Viewer delivers by exploring the creative work of its developer, William Weaver's [YouTube Channel](https://www.youtube.com/@colorscompletely/featured) and photobooks:

[![none of this is real - photobook - Click to Open PDF](https://github.com/williamweaver/Aperture-Viewer/blob/dev/doc/none_of_this_is_real_cover.png)](https://github.com/williamweaver/Aperture-Viewer/raw/dev/doc/none_of_this_is_real_Optimized.pdf)

[![we all have this madness - photobook - Click to Open PDF](https://github.com/ApertureViewer/Aperture-Viewer/blob/dev/doc/we_all_have_this_madness_cover.png)](https://github.com/ApertureViewer/Aperture-Viewer/raw/dev/doc/we_all_have_this_madness.pdf)

<br/>

> [!IMPORTANT]
> **Disclaimer:** Disclaimer: Aperture Viewer is an independent project. While now an **officially listed Third-Party Viewer for Second Life**, it is not directly supported by Linden Lab beyond their standard open-source policies and TPV program guidelines. While built on Linden Lab's and Firestorm's open-source code, it offers a unique feature set and development direction. Users should ensure their system meets requirements and understand that features may differ from other viewers.
>*   **Open Source Foundation:** Derived from the official [Second Life](https://github.com/secondlife/viewer) client via the Firestorm Viewer codebase. Aperture Viewer continues this open-source tradition.
>*   **Hardware Considerations:** Advanced visual features, especially the Post-Processing pipeline and higher Graphics Preset levels, may require capable computer hardware for optimal performance.

---

## Aperture Viewer 1.0.0: Key Features & Enhancements

Aperture Viewer is engineered to provide sophisticated tools and unparalleled control. Version 1.0.0 introduces a suite of groundbreaking features:

*   **All-New Aperture Phototools Suite (APS) `(Exclusive)`:** The heart of Aperture 1.0.0 (Alt+P). This completely rebuilt, unified floater provides unparalleled real-time control over your virtual world visuals via intuitive tabs, integrating many core features:
    *   **Integrated Environment Editing (Evn Tab):** Full Sky/Water preset management, quick Windlight adjustments, and detailed, real-time editing of Atmosphere, Cloud, Sun/Moon, and Water parameters. Optionally hide non-user presets.
    *   **Revolutionary Post-Processing Pipeline (Post Tab):** Access groundbreaking real-time image mastering tools (detailed below).
    *   **Lighting & Shadows (Shd Tab):** Fine-tune shadows (resolution, clarity, blur, bias) and Screen Space Ambient Occlusion (SSAO), including **real-time SSAO Sample Count control**.
    *   **Reflections (Refl Tab):** Manage Reflection Probes (up to **2048px resolution**), Screen Space Reflections (SSR), and Mirrors.
    *   **Lens Effects (Lens Tab):** Control Depth of Field (DoF), Glow/Bloom, and **Chromatic Aberration**.
    *   **Enhanced Avatar Controls (Ava Tab):** Manage avatar rendering, complexity, physics LOD, attached lights/particles, **Hover Height**, and **Fluid Global Animation Speed**. Includes a 'Force Rebake' button.
    *   **Graphics Management (Gen Tab):** Select performance-tuned **Graphics Levels**, Save/Load/Delete **Custom Graphics Presets** (capturing the full visual state including PostFX), control Draw Distance, LOD factors, Fullbright, Aniso, Occlusion, and dynamically scale the entire **User Interface (UI Scale)**.
    *   **Camera Controls & Presets (Cam Tab):** Integrated standard viewer camera controls (Orbit/Pan/Zoom/Roll), standard views, 3D Mouse/Joystick settings, and the new **Camera Preset System** (12 Standard + 12 Flycam slots).

*   **Groundbreaking Post-Processing Pipeline `(Exclusive)`:** Integrated within APS ('Post' tab), achieve **professional-grade image mastering *live* within the viewer**, rivaling tools like Adobe Camera Raw/Lightroom.
    *   **Photographic Tone Mapping:** Independent control over Exposure, Contrast, Highlights, Shadows, Whites, Blacks, Crush Blacks.
    *   **Complete Color Grading Suite:** Vibrance, Saturation, customizable Luminance Weights, and **3-Way Color Balance** (Shadows/Midtones/Highlights) w/ Luma Preservation.
    *   **Cinematic Effects:** High-quality adjustable **Film Grain** and **Chromatic Aberration**.

*   **Vastly Improved Procedural Starfield `(Exclusive)`:** Replaces the ~20-year-old legacy system with a realistic night sky featuring hundreds of thousands of stars, **black body coloration**, luminosity distribution, **dynamic atmospheric twinkling**, and horizon extinction effects.

*   **Performance-Tuned Graphics Levels & Comprehensive Custom Presets `(Exclusive System)`:**
    *   **9 Feature-Driven Levels:** (via a **custom `featuretable_aperture.txt`**) provide balanced performance starting points.
    *   **Comprehensive Custom Presets:** Save/Load feature captures the **entire visual state**, including **all Post-Processing settings**, allowing creation and recall of complete visual styles.

*   **Robust Camera Preset System `(Exclusive)`:** Save and recall **12 Standard + 12 Flycam** camera views (Position [Global Coords], Focus, Roll Angle) instantly via the APS 'Cam' tab.

*   **New Star Wars Themed Skins `(Exclusive)`:** Four new themes (Vader, Yoda, X-wing, Obiwan) join the existing exclusive dark themes. Default theme: **Phantom**.

*   **Enhanced Performance & Creator Tools:**
    *   **Faster Startup:** Hardware probes disabled.
    *   **Modernized Defaults:** Higher network bandwidth, larger caches, refined VSync/FPS Limiter logic.
    *   **Increased Limits:** Reflection Probe resolution up to **2048px**, Camera FOV **0.1°–179.9°**, Object/Volume LOD defaults to **8.0** (preventing common accessory deformation).
    *   **UI/QoL:** **Poser** & **Teleport History** buttons on toolbar, improved tab tooltips, simplified progress UI.
    *   **Privacy Focus:** Dynamic data downloads disabled, local chat logging default, voice/lookat targets disabled default.

[Explore the full range of innovations on the Aperture Viewer Features Overview (Wiki).](https://github.com/williamweaver/Aperture-Viewer/wiki/Viewer-Features)

---

## Aperture Viewer Wiki: Your Comprehensive Guide & Contribution Hub

Dive into the **[Aperture Viewer Wiki](https://github.com/williamweaver/Aperture-Viewer/wiki/Home)** for in-depth documentation on all features and to learn how to make the most of your creative experience. *(Note: Wiki pages are being actively updated for v1.0.0)*

> [!TIP]
> **New to Aperture Viewer?** We recommend starting with the **[Download & Installation Guide](https://github.com/williamweaver/Aperture-Viewer/wiki/Download-&-Installation)** and the **[Getting Started with Aperture Viewer](https://github.com/williamweaver/Aperture-Viewer/wiki/Getting-Started)** page. You'll also find essential **[Troubleshooting & FAQ](https://github.com/williamweaver/Aperture-Viewer/wiki/Troubleshooting)** information.

The Wiki is your central resource for:

*   **Mastering the Aperture Phototools Suite (APS):** Explore the **[APS - Overview](https://github.com/williamweaver/Aperture-Viewer/wiki/Aperture-Phototools-Suite)** and detailed guides for each tab, including the revolutionary **[Post-Processing Effects](https://github.com/williamweaver/Aperture-Viewer/wiki/Post-Tab-‐-Post‐Processing-Effects)**, comprehensive **[Environment Settings](https://github.com/williamweaver/Aperture-Viewer/wiki/Env-Tab-‐-Environment-Settings)**, and precise **[Camera Controls & Presets (within APS)](https://github.com/williamweaver/Aperture-Viewer/wiki/Cam-Tab-‐-Camera-Controls)**.
*   **Understanding Key Systems:** Learn about Aperture's unique **[Graphics Presets System](https://github.com/williamweaver/Aperture-Viewer/wiki/Aperture-Graphics-Presets)** and the dedicated **[Camera Presets System (within APS)](https://github.com/williamweaver/Aperture-Viewer/wiki/Aperture-Camera-Presets)**.
*   **Image Making How-To Guides:** Enhance your skills with practical tutorials like the **[Black & White Photography in APS](https://github.com/williamweaver/Aperture-Viewer/wiki/Black-&-White-Photography)** guide.
*   **Technical Deep Dives:** Find analyses of core **[Second Life Lighting](https://github.com/williamweaver/Aperture-Viewer/wiki/Second-Life-Lighting-System)** and **[Shadow Systems](https://github.com/williamweaver/Aperture-Viewer/wiki/Second-Life-Shadow-System)**, plus explanations of important viewer settings like **[Cache Settings](https://github.com/williamweaver/Aperture-Viewer/wiki/Cache-Settings)** and **[Connection Speed Settings](https://github.com/williamweaver/Aperture-Viewer/wiki/Connection-Speed-Setting)**. Other deep dives include **[Glow Effects](https://github.com/williamweaver/Aperture-Viewer/wiki/Glow-Effect-Settings)**, **[RenderDynamicLOD](https://github.com/williamweaver/Aperture-Viewer/wiki/RenderDynamicLOD)**, **[PBR Emissive Buffer](https://github.com/williamweaver/Aperture-Viewer/wiki/RenderEnableEmissiveBuffer)**, **[HDR Pipeline](https://github.com/williamweaver/Aperture-Viewer/wiki/RenderHDREnabled)**, **[Reflection Probe Levels](https://github.com/williamweaver/Aperture-Viewer/wiki/RenderReflectionProbeLevel)**, **[Object Detail (RenderVolumeLODFactor)](https://github.com/williamweaver/Aperture-Viewer/wiki/RenderVolumeLODFactor)**, **[SSAO](https://github.com/williamweaver/Aperture-Viewer/wiki/Screen-Space-Ambient-Occlusion)**, **[SSR](https://github.com/williamweaver/Aperture-Viewer/wiki/Screen-Space-Reflections)**, **[Tone Mapping Fidelity](https://github.com/williamweaver/Aperture-Viewer/wiki/Enhancing‐Tone‐Mapping‐Fidelity)**, and **[Shadow Update Analysis](https://github.com/williamweaver/Aperture-Viewer/wiki/RenderShadowResolutionScale-Fixing-Shadow-Updates-After-Graphics-Setting-Changes)**.
*   **Project Information:** Learn about the viewer's **[History and Development](https://github.com/williamweaver/Aperture-Viewer/wiki/History-and-Development)**, view the **[Change Log](https://github.com/williamweaver/Aperture-Viewer/wiki/Change-Log)**, see **[Credits & Acknowledgements](https://github.com/williamweaver/Aperture-Viewer/wiki/Credits-&-Acknowledgements)**, and find out how to **[Contact & Support](https://github.com/williamweaver/Aperture-Viewer/wiki/Contact-&-Support)** us.
*   **Contributing:** Discover **[Ways to Contribute](https://github.com/williamweaver/Aperture-Viewer/wiki/Ways-to-Contribute)**, including guidelines for **[Contributing Code](https://github.com/williamweaver/Aperture-Viewer/wiki/Contributing-Code-to-Aperture-Viewer)** and **[Building from Source (Windows)](https://github.com/williamweaver/Aperture-Viewer/wiki/Building-from-Source-‐-(Windows))**.
*   **Policies:** Review our **[Privacy Policy](https://github.com/williamweaver/Aperture-Viewer/wiki/Privacy-Policy)** and **[Terms of Service](https://github.com/williamweaver/Aperture-Viewer/wiki/Terms-of-Service)**.

The Wiki is a living document, continually evolving with the viewer. We encourage you to explore and use it as your go-to reference. You can find the main [Wiki Home page here](https://github.com/williamweaver/Aperture-Viewer/wiki/Home) for a full table of contents via its sidebar.

---

## Credits

Aperture Viewer is built upon the robust foundation laid by the **Firestorm Viewer Project**. This independent project would not be possible without the immense and ongoing work of the Firestorm team in creating and maintaining their open-source viewer codebase. We gratefully acknowledge their significant contributions to the Second Life and OpenSim communities.

We also extend our sincere appreciation to **Linden Lab** for their foundational commitment to open source and for creating the Second Life platform that enables independent projects like Aperture Viewer to flourish.

Aperture Viewer is maintained by William Weaver. The advanced Phototools system, Post-Processing pipeline, Procedural Stars, Graphics Level system, and Camera Preset systems build upon original concepts and design work by William Weaver.

[For detailed credits and acknowledgements, please see the Wiki Credits & Acknowledgements page.](https://github.com/williamweaver/Aperture-Viewer/wiki/Credits-&-Acknowledgements)

---

## Download Aperture Viewer v1.0.0 (Windows AVX Release)

> [!WARNING]
> **Windows AVX Release:** The official v1.0.0 release is optimized for **Windows 64-bit systems with AVX instruction set compatible processors.** Please ensure your system meets this requirement. Releases for other platforms are not currently available.

[**Download v1.0.0 from GitHub Releases**](https://github.com/ApertureViewer/Aperture-Viewer/releases)

This release provides access to the latest stable features of Aperture Viewer.

## Build Instructions

Aperture Viewer can be built from source. Refer to the Aperture-specific instructions for critical differences from standard Firestorm builds.

*   [Windows Build Instructions](https://github.com/williamweaver/Aperture-Viewer/wiki/Building-from-Source-‐-(Windows))
*   *(Note: Mac and Linux build instructions require significant adaptation and are not currently maintained for Aperture-specific changes.)*

> [!CAUTION]
> **Limited Support for Self-Compiles:** As with Firestorm, official support for self-compiled versions of Aperture Viewer or issues arising from them is limited.

---

## Privacy Commitment - User Data and Transparency

**Aperture Viewer is designed with user privacy as a core principle.** We are committed to transparency regarding your data:

*   **No User Data Collection:** Aperture Viewer **does not collect any personal data** related to your usage.
*   **No Data Collection Infrastructure:** We do not operate server infrastructure to collect or store user data.
*   **Open Source Transparency:** Aperture Viewer is **open source.** Review the source code in this repository to verify our commitment.

**For detailed information, please review our full [Privacy Policy](https://github.com/williamweaver/Aperture-Viewer/wiki/Privacy-Policy) and [Terms of Service](https://github.com/williamweaver/Aperture-Viewer/wiki/Terms-of-Service).**

---

## Join the Aperture Community

Aperture Viewer is committed to upholding the principles of responsible open-source development, adhering to platform policies, and fostering a community dedicated to visual excellence.

**Ready to elevate your virtual creations? Download Aperture Viewer 1.0.0 today and experience the difference.**

*   **View** our official listing on the **[Second Life Third-Party Viewer Directory](https://wiki.secondlife.com/wiki/Third_Party_Viewer_Directory/Aperture_Viewer)**.
*   **Explore** the [Aperture Viewer Repository (Code)](https://github.com/williamweaver/Aperture-Viewer) and the **[Aperture Viewer Wiki](https://github.com/williamweaver/Aperture-Viewer/wiki/Home)**.
*   **Download** the latest release: **[Aperture Viewer v1.0.0 from GitHub Releases](https://github.com/ApertureViewer/Aperture-Viewer/releases/tag/1.0.0)**
*   **Contribute** your feedback, suggestions, or code via **[GitHub Issues](https://github.com/williamweaver/Aperture-Viewer/issues)**.
*   **Join** our community on **[Discord](https://discord.gg/rVmPcUgF7Z)**!


---
