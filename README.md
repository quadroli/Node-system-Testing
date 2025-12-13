## External Project Dependencies

This project requires the following third-party libraries. They must be downloaded and placed in a `lib/` directory before compilation. You must create the `lib/` folder in the exact same folder as the rest of the project files.

---

### 1. Dear ImGui (v1.89)

The core graphical user interface library.

* **Required Version:** v1.89 (or newer, but tested with 1.89)
* **Source:** [https://github.com/ocornut/imgui](https://github.com/ocornut/imgui)
* **Purpose:** Provides the immediate mode GUI elements.
* **Files Required:** The entire contents of the `imgui` repository, including the `backends/` folder.
* **Installation Note:** After downloading, move the `imgui-master` folder the `lib/` folder after downloading and do not reorder or rename anything.

### 2. GLFW (v3.4)

A multi-platform library for creating windows, contexts, and handling input.

* **Required Version:** v3.4
* **Source:** [https://www.glfw.org/download.html](https://www.glfw.org/download.html)
* **Purpose:** Handles the window creation and input for the ImGui backend.
* **Installation Note:** After downloading, move the `glfw-3.4` folder the `lib/` folder after downloading and do not reorder or rename anything.

### 3. GLEW (The OpenGL Extension Wrangler)

Provides access to modern OpenGL functions (like shaders, VBOs, etc.).

* **Required Version:** v2.1.0
* **Source:** [http://glew.sourceforge.net/](http://glew.sourceforge.net/)
* **Purpose:** Initializes all OpenGL functions and extensions required by the ImGui backend and your custom rendering code.
* **Installation Note:** After downloading, move the `glew-2.1.0` folder the `lib/` folder after downloading and do not reorder or rename anything.

### 4. OpenGL (Included with OS)

The rendering API used by ImGui's backend.

* **Required Version:** OpenGL 3.3 Core Profile (or higher)
* **Source:** N/A (Part of your operating system/graphics driver)
* **Purpose:** Drawing and rendering the ImGui geometry.
* **Installation Note:** Ensure your graphics drivers are up to date.

---

## Compilation and Execution
Run `cmake . -B . .\BUILD\`, then `cmake --build .\BUILD\` to compile the project. If it isn't obvious, you kinda need to have CMake installed for this to work.

In order to run the project, run `BUILD/Debug/Bambuu.exe`