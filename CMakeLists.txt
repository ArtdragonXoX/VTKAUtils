file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.h)

add_library(VTKAUtils ${srcs})

find_package(VTK REQUIRED)

target_link_libraries(VTKAUtils
    ${VTK_LIBRARIES}
)

target_include_directories(VTKAUtils PUBLIC
    include
)