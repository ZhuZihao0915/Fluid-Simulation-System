
enable_language(C CXX)

file(GLOB_RECURSE SPH2D_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE SPH2D_HEADER_FILES "./include/*.h ./include/*.hpp")

source_group("Header Files" FILES ${SPH2D_HEADER_FILES})

add_library(sph2d STATIC "${SPH2D_SOURCE_FILES}" "${SPH2D_HEADER_FILES}")
target_include_directories(sph2d PRIVATE "./include")

# common
target_link_libraries(sph2d common)

# glfw
target_link_libraries(sph2d "${PROJECT_SOURCE_DIR}/third_party/glfw/lib/glfw3.lib")