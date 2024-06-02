# Vulkan Terrain Renderer

![](screenshots/banner.png)

Simple procedural terrain renderer.

It uses perlin noise for heightmaps, tessellation for dynamic LOD and SSR for water reflections.

# Sample video (click image)

[![](screenshots/water.gif)](https://www.youtube.com/watch?v=hysko2e0ro0)


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

For windows, you can open it with Visual Studio as a CMake Project.