# QImageAnimator

QImageAnimator lets you record screenshots of your monitor
that can be later converted into an animated image.

At the moment only GIF file output is supported but
WebP is the next main target on the roadmap.

The application mainly targets the Windows Os, but should
work as well on Linux (quite trivially) and Mac OS (not so trivially)
with some little effort.


# Roadmap

1. Clean up UI interactions and user feedback (looking at you start/stop)
2. Load images from folder to support reusing previously recorded frames
3. Delegate conversion to dedicated thread and notify the pending status
   rather than simply block the UI :/
4. WebP support

# Build

Git, CMake, Qt5 (tested on 5.15.2), Visual Studio (MSVC compiler) are needed
to build on Windows.

1. Clone the repo
2. create a `build` directory and enter it
3. run cmake using `CMAKE_PREFIX_PATH` to point to
   the cmake folder of your Qt installation directory, eg:

   `cmake -DCMAKE_PREFIX_PATH=C:/Qt/5.15.2/msvc2019_64/lib/cmake ..`

4. open the Visual Studio solution, set QImageAnimator as default project
   and build it.

   For a Release build change the project settings accordingly.

5. to install, back to the CLI within the `build` directory and type

   `cmake --install . --prefix <path-to-install>`
