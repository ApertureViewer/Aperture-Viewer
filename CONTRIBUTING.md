First off, thank you for considering contributing to Aperture Viewer! We aim to significantly push the boundaries of visual fidelity and creator workflow efficiency in virtual worlds, and every contribution helps us achieve that goal.

**We recognize that our contribution guidelines, particularly for Pull Requests and commit messages, are quite detailed and may seem more structured, perhaps even demanding, compared to those of some other projects. This level of detail is intentional and deeply valued. As a small, volunteer-led team with limited resources, maintaining a robust, well-documented, and consistent development process is absolutely essential for us. Adherence to these specific conventions allows us to efficiently manage contributions, track project progress accurately, ensure long-term maintainability, and ultimately, make the most of every valuable contribution. We believe these practices, rooted in principles of sound project management and quality assurance, help us build a high-quality viewer and respect the time and effort of everyone involved.**

**That said, when considering if, how, or when you should contribute, please understand that we strive to be an exceptionally welcoming, friendly, and understanding community. We genuinely enjoy working with people and thrive on collaboration. We recognize that everyone is at different stages in their learning journey. So, if you have questions, need help, have never contributed to an open-source project before, or simply want some guidance, please don't hesitate to reach out to us (especially on Discord – see the [Questions?](#questions) section below for links). Give us a chance to assist you; our aim is to foster a warm, supportive environment for all contributors.**

This document provides guidelines for contributing to the Aperture Viewer project, with a primary focus on code contributions. Please read it carefully to ensure a smooth and effective collaboration.

## Table of Contents

*   [Project Vision](#project-vision)
*   [How Can I Contribute?](#how-can-i-contribute)
*   [Getting Started with Code Contributions](#getting-started-with-code-contributions)
    *   [Prerequisites](#prerequisites)
    *   [Forking and Cloning](#forking-and-cloning)
    *   [Branching Strategy](#branching-strategy)
*   [Making Changes (Code Contributions)](#making-changes-code-contributions)
    *   [Coding Conventions](#coding-conventions)
    *   [Commit Message Guidelines (Strictly Enforced)](#commit-message-guidelines-strictly-enforced)
    *   [Local Testing](#local-testing)
*   [Submitting a Pull Request (PR)](#submitting-a-pull-request-pr)
    *   [PR Checklist](#pr-checklist)
    *   [What to Expect After Submitting](#what-to-expect-after-submitting)
*   [License](#license)
*   [Questions?](#questions)

## Project Vision

Our vision is "Nothing Is Not Possible" – delivering cutting-edge, exclusive visual features and advanced photographic/videographic tools integrated directly into the viewer. We prioritize innovation, visual excellence, performance, and a creator-focused approach.

## How Can I Contribute?

There are many ways you can help Aperture Viewer:

*   **Reporting Bugs:** If you find a bug, please first search our [GitHub Issues](https://github.com/ApertureViewer/Aperture-Viewer/issues) to ensure it hasn't already been reported. If not, [open a new issue](https://github.com/ApertureViewer/Aperture-Viewer/issues/new) using the "Bug report" template if available. Provide a clear title, detailed description, and precise steps to reproduce the issue. Include Aperture Viewer version, OS, graphics card, and screenshots/logs if helpful.
*   **Suggesting Enhancements or New Features:** We welcome ideas! Please check [GitHub Issues](https://github.com/ApertureViewer/Aperture-Viewer/issues) for existing suggestions. If your idea is new, [open a new issue](https://github.com/ApertureViewer/Aperture-Viewer/issues/new) using the "Feature request" template if available. Clearly describe the proposal, its benefits (especially for photography/videomaking, but also general usability), and potential use cases.
*   **Contributing Code:** While the core Aperture Viewer development often emphasizes advancements in visual creation, photography, videomaking, rendering quality, and the Phototools suite, we warmly welcome and encourage code contributions of **all types** that are beneficial to the broader Second Life and OpenSimulator user community. This can include performance improvements, UI/UX refinements, bug fixes, privacy enhancements, or new general-purpose features. If your contribution improves the viewer experience, we're interested! Please see [Getting Started with Code Contributions](#getting-started-with-code-contributions) below for technical guidelines.
*   **Improving Documentation:** Our Wiki needs your help! For details on suggesting documentation changes or reporting errors, please see our [Wiki Contribution Guide](https://github.com/ApertureViewer/Aperture-Viewer/wiki/Contributing-to-the-Wiki).
*   **Testing and Providing Feedback:** Your feedback, especially on alpha/beta releases, is invaluable. Please submit feedback via [GitHub Issues](https://github.com/ApertureViewer/Aperture-Viewer/issues) or our official community channels on [Discord](https://discord.gg/9a3x364f).

## Getting Started with Code Contributions

### Prerequisites

*   Familiarity with C++, GLSL shaders, and Git.
*   A working build environment for Second Life viewers (CMake-based). Refer to general Firestorm or Second Life viewer build guides for initial setup if needed. *(Consider linking to a `BUILDING.md` in your repo if you create one with Aperture-specific setup notes).*
*   An IDE or text editor of your choice. Configure your editor to use spaces for indentation.

### Forking and Cloning

1.  **Fork** the `ApertureViewer/Aperture-Viewer` repository to your own GitHub account.
2.  **Clone** your fork to your local machine:
    ```bash
    git clone https://github.com/YOUR_USERNAME/Aperture-Viewer.git
    cd Aperture-Viewer
    ```
3.  **Add the upstream repository** to keep your fork synced:
    ```bash
    git remote add upstream https://github.com/ApertureViewer/Aperture-Viewer.git
    ```

### Branching Strategy

*   **ALL Aperture-specific development happens on branches created from `dev`.**
*   Before starting any new work, ensure your local `dev` branch is up-to-date with `upstream/dev`:
    ```bash
    git checkout dev
    git fetch upstream
    git rebase upstream/dev # Preferred to keep history clean, or use 'git merge upstream/dev'
    git push origin dev --force-with-lease # If rebased, otherwise 'git push origin dev'
    ```
    *(Note: `rebase` rewrites history. If you're collaborating on a `dev` branch with others on your fork, coordinate or use `merge`.)*
*   Create a new feature or fix branch from `dev`:
    *   For new features: `git checkout -b feature/your-descriptive-feature-name`
    *   For bug fixes: `git checkout -b fix/issue-number-or-description`
*   **NEVER commit directly to `master` or `dev` in your local repository if you intend to PR from it. Always use a feature/fix branch.** The `master` branch in `ApertureViewer/Aperture-Viewer` is for pristine upstream Firestorm code.

## Making Changes (Code Contributions)

### Coding Conventions

1.  **Aperture Commenting Style (Mandatory for all Aperture-specific code):**
    *   Multi-line Aperture-specific code blocks:
        ```cpp
        // <AP:YOUR_TAG>
        // Your new code or detailed explanation here...
        // ...more lines if needed.
        // </AP:YOUR_TAG>
        ```
        Replace `YOUR_TAG` with your initials or GitHub username (e.g., `<AP:JD>` for John Doe). The project lead uses `<AP:WW>`.
    *   Single-line Aperture-specific changes:
        ```cpp
        int example = oldValue; // <AP:JD/> Changed for new feature X.
        ```
    *   When modifying/replacing existing Firestorm/Linden Lab code, comment out the original within `<FS:YOUR_TAG>...</FS:YOUR_TAG>` blocks:
        ```cpp
        // <FS:JD>
        // original_firestorm_code_line_1();
        // original_firestorm_code_line_2();
        // </FS:JD>
        // <AP:JD>
        my_new_aperture_code();
        // </AP:JD>
        ```
    *   **Rationale:** Clear attribution and differentiation of Aperture code from upstream code is critical for maintainability, understanding, and potential future merges or PRs.
2.  **General Code Style:**
    *   **Indentation: Use spaces, not tabs.** This aligns with Linden Lab and Firestorm coding conventions. Configure your editor accordingly (typically 4 spaces per indent level, but match existing code).
    *   Follow the existing style in the Firestorm codebase for other aspects like bracing, spacing around operators, and naming conventions for consistency.
    *   When in doubt, prioritize clarity and readability.
    *   For general good practices in viewer development, we also recommend reviewing the [Firestorm Pull Request Guidelines](https://github.com/FirestormViewer/phoenix-firestorm/blob/master/doc/FS_PR_GUIDELINES.md), though Aperture-specific guidelines in *this* document (especially commit messages and Aperture commenting) take precedence.
3.  **File Type Specific Commenting (General Comments):**
    *   `.cpp`, `.h`, `CMakeLists.txt`, `.rc`: Use `//`
    *   `.py`: Use `#`
    *   `.sh`: Use `#`
    *   `.nsi`: Use `;` or `#`

### Commit Message Guidelines (Strictly Enforced)

We require detailed and consistently formatted commit messages. This is crucial for project history, changelog generation, and understanding changes.

**Format:**
Type: Concise and accurate summary (max ~72 chars)

(Blank Line)

Body:
- Provide a detailed description of WHAT changed and, crucially, WHY the change was made.
- Explain the problem being solved or the feature being added.
- Reference issue tracker IDs if applicable (e.g., "This addresses feedback from Issue #7." or "Part of implementing feature outlined in #12.").
- Use bullet points for clarity if multiple distinct changes are included.
- Be verbose and explain your rationale.

(Blank Line)

Testing:
- Describe HOW the changes should be tested by a reviewer or QA. Be specific about steps.
- Mention the testing environment if relevant (e.g., "Tested on Windows 11 with NVIDIA RTX 4080").
- Include specific scenarios or edge cases to check.
- This section MUST be detailed enough for someone else (or future you) to reproduce the tests.

(Blank Line, Optional)

Documentation: (Include this section only if applicable)
- Suggest specific updates needed for user documentation, wiki pages, or internal docs as a result of the change (especially for Feature or significant Enhance commits).
- Example: Documentation: Update Phototools wiki page to include new XYZ control. Detail changes to Graphics Preset Level 9 defaults.


**Allowed Commit Types (Title must start with one of these):**

*   `Refactor:` (Code restructuring without changing external behavior)
*   `Feature:` (New, user-visible functionality)
*   `Enhance:` (Improvement or extension of existing functionality)
*   `Fixes:` (Bug fixes)
*   `Docs:` (Documentation updates)
*   `Build:` (Changes to build system, scripts, packaging)
*   `Chore:` (Maintenance, cleanup, non-code tasks)
*   `Perf:` (Performance improvements)
*   `Style:` (Code style changes only, no functional change)
*   `Test:` (Adding or correcting tests)

```

**Example Commit Message:**

Feature: Add exposure compensation slider to Phototools

This commit introduces a new slider for exposure compensation within the
Aperture Phototools Suite (APS). This allows users to make real-time
adjustments to the overall brightness of the scene before other post-processing
effects are applied.

The slider ranges from -2.0 to +2.0 EV. The shader uniform apExposureCompensation
has been added to the main post-processing shader.

Addresses request in Issue #42.

Testing:
- Open Phototools (Ctrl+Shift+P).
- Navigate to the "Tone Mapping" section.
- Verify the "Exposure Comp" slider is present.
- Move the slider left and right. Confirm the scene brightness changes accordingly.
- Test with various EEP settings and windlights.
- Ensure the value is saved and restored correctly with Graphics Presets.
- Tested on Windows 10, NVIDIA GTX 1080.

Documentation:
- Update Phototools wiki page to document the new Exposure Compensation slider.
- Add to tutorial on basic image adjustments.

```
      
### Local Testing

*   **Thoroughly test your changes locally before committing and pushing.**
*   Rebuild the viewer and run it to ensure your changes work as expected and haven't introduced regressions.
*   Consider different scenarios and edge cases. Pay special attention to how your changes might affect visual fidelity and performance.

## Submitting a Pull Request (PR)

Once your changes are complete, tested, and committed with proper messages on your feature/fix branch:

1.  Push your branch to your fork on GitHub:
    ```bash
    git push -u origin feature/your-descriptive-feature-name
    ```
2.  Go to the `ApertureViewer/Aperture-Viewer` repository on GitHub.
3.  You should see a prompt to create a Pull Request from your recently pushed branch. If not, navigate to the "Pull requests" tab and click "New pull request".
4.  Ensure the **base repository** is `ApertureViewer/Aperture-Viewer` and the **base branch** is `dev`.
5.  Ensure the **head repository** is your fork and the **compare branch** is your feature/fix branch.

### PR Checklist

Before submitting your PR, please ensure the following:

*   [ ] **Clear Title:** The PR title should be concise and clearly describe the change (e.g., "Feature: Add Japanese Translation Files" or "Fixes: #123 - Crash on opening inventory"). The title should ideally follow the commit type convention (e.g., "Feature: ...", "Fixes: ...").
*   [ ] **Detailed Description:**
    *   Explain *what* the PR does and *why* these changes are being made. This can often be a summary of your commit message bodies.
    *   Link to any relevant GitHub Issues (e.g., "Fixes #123", "Closes #124", "Addresses #125"). This helps with tracking and auto-closing issues.
*   [ ] **Testing Information:**
    *   Clearly describe how you tested your changes.
    *   Provide specific steps for reviewers/QA to verify the changes.
    *   **For UI changes, visual features, or translations:** Please provide specific areas to check. If possible (especially for translations or complex UI), include screenshots showing the changes in action. Confirm that text fits within UI elements, and there are no obvious layout issues.
    *   If you are providing a translation, please confirm you have tested the viewer with the language selected and that strings display correctly.
*   [ ] **Adherence to Guidelines:** ** Confirm your code follows the [Coding Conventions](#coding-conventions) (including using spaces for indentation) and ALL your commits follow the [Commit Message Guidelines](#commit-message-guidelines-strictly-enforced).`
*   [ ] **Self-Review:** Review your own diff (`Files changed` tab on the PR) to catch any typos, leftover debug code, or unintended changes.
*   [ ] **Single Purpose (Ideally):** PRs should ideally address a single issue or feature. If you have multiple unrelated changes, please submit them as separate PRs.
*   [ ] **Working Code:** Your code compiles, runs, and has been tested by you.
*   [ ] **Up-to-Date Branch:** Your feature/fix branch is based on the latest `dev` branch of `ApertureViewer/Aperture-Viewer`. If `dev` has been updated since you created your branch, rebase your branch onto the latest `dev` before submitting the PR.

### What to Expect After Submitting

*   The project lead or other contributors will review your PR.
*   We may ask questions, request changes, or provide feedback. Please be responsive to comments. Constructive discussion is welcome.
*   Once the PR is approved and passes any automated checks (if configured), it will be merged into `dev`.
*   Your branch on GitHub can be deleted after the PR is merged.

## License

By contributing to Aperture Viewer, you agree that your contributions will be licensed under the GNU Lesser General Public License, version 2.1 (LGPL v2.1), which is inherited from Firestorm and the official Linden Lab Second Life Viewer. You can find a copy of the license in the `LICENSE.txt` file in this repository.

## Questions?

If you have any questions about the contribution process, **no matter how basic they might seem, or if you're new to contributing and need guidance,** please feel free to:
*   Open an issue on GitHub and tag it with "question".
*   Reach out on our official communication channels ([Discord](https://discord.gg/9a3x364f).). **We are happy to help and aim to be a friendly, welcoming community for everyone at all skill levels.**

Thank you for contributing to making Aperture Viewer awesome!