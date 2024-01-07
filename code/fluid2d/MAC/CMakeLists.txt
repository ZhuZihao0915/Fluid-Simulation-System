
enable_language(C CXX)

file(GLOB_RECURSE MAC2D_SOURCE_FILES "./src/*.cpp")
file(GLOB_RECURSE MAC2D_HEADER_FILES "./include/*.h ./include/*.hpp")

source_group("Header Files" FILES ${MAC2D_HEADER_FILES})

add_library(mac2d STATIC "${MAC2D_SOURCE_FILES}" "${MAC2D_HEADER_FILES}")
target_include_directories(mac2d PRIVATE "./include")

include_directories("./include")

# common
target_link_libraries(mac2d common)

# glfw
target_link_libraries(mac2d "${PROJECT_SOURCE_DIR}/third_party/glfw/lib/glfw3.lib")

# freeglut
target_link_libraries(mac2d "${PROJECT_SOURCE_DIR}/third_party/freeglut/lib/freeglut.lib")
target_link_libraries(mac2d "${PROJECT_SOURCE_DIR}/third_party/freeglut/lib/freeglutd.lib")