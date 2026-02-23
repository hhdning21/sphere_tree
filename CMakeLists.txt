cmake_minimum_required(VERSION 3.10)
project(SphereTree VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main library
add_library(spheretree STATIC
    src/SphereTree.cpp
    src/SphereTreeAPI.cpp
)

# Shared library
add_library(spheretree_shared SHARED
    src/SphereTree.cpp
    src/SphereTreeAPI.cpp
)

set_target_properties(spheretree_shared PROPERTIES OUTPUT_NAME spheretree)

target_include_directories(spheretree PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(spheretree_shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(spheretree_shared PROPERTIES
    OUTPUT_NAME spheretree
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

# Examples
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_executable(example_usage examples/example_usage.cpp)
    target_link_libraries(example_usage spheretree)
    
    add_executable(example_obj_loader examples/example_obj_loader.cpp)
    target_link_libraries(example_obj_loader spheretree)
    
    add_executable(example_simple_api examples/example_simple_api.cpp)
    target_link_libraries(example_simple_api spheretree)
    
    # OpenGL visualization (optional - only if OpenGL/GLUT available)
    if(APPLE)
        # On macOS, use frameworks directly
        message(STATUS "Building OpenGL visualization tool (macOS)")
        add_executable(visualize_spheres_gl examples/visualize_spheres_gl.cpp)
        target_link_libraries(visualize_spheres_gl spheretree "-framework OpenGL" "-framework GLUT")
    elseif(UNIX AND NOT APPLE)
        find_package(OpenGL)
        find_package(GLUT)
        if(OPENGL_FOUND AND GLUT_FOUND)
            message(STATUS "Building OpenGL visualization tool")
            add_executable(visualize_spheres_gl examples/visualize_spheres_gl.cpp)
            target_link_libraries(visualize_spheres_gl spheretree ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
        endif()
    endif()
endif()

# Installation
install(TARGETS spheretree spheretree_shared
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)
