project (imgui)

file(GLOB_RECURSE SRC
     "*.cpp"
)


add_library(imgui STATIC)
find_package(Vulkan REQUIRED)


target_include_directories(imgui PUBLIC include ${Vulkan_INCLUDE_DIRS})

target_link_libraries(imgui PUBLIC
    ${Vulkan_LIBRARIES}
    SDL3::SDL3
)

target_sources(imgui PRIVATE ${SRC})