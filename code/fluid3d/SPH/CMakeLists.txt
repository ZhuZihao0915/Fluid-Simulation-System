
enable_language(C CXX)

file(GLOB_RECURSE SPH3D_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE SPH3D_HEADER_FILES "./include/*.h ./include/*.hpp")

source_group("Header Files" FILES ${SPH3D_HEADER_FILES})

add_library(sph3d STATIC "${SPH3D_SOURCE_FILES}" "${SPH3D_HEADER_FILES}")
target_include_directories(sph3d PRIVATE "./include")

# common
target_link_libraries(sph3d common)

# glfw
target_link_libraries(sph3d "${PROJECT_SOURCE_DIR}/third_party/glfw/lib/glfw3.lib")