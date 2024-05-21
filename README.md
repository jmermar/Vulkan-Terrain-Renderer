# Vulkan Terrain Renderer

![](screenshots/banner.png)

WIP procedural terrain renderer that uses tessellation for dynamic LOD and simplex noise for terrain generation.

# Build

The project uses CMake and have been built both on linux (Ubuntu) and Windows 10.

In order to compile it you'll need Vulkan SDL installed, CMake, and a c++ compiler (msvc++ on windows or g++ on linux tested)

Instructions for Linux:

```
mkdir build &
cd build &
cmake .. &
make
```

For windows, you can open it with visual studio as a CMake Project.