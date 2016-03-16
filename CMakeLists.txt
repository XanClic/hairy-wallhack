cmake_minimum_required( VERSION 2.8 )

project(audiophile)

include(ExternalProject)
include(FindPkgConfig)

include_directories( include )

add_definitions("--std=c++11 -Wall -Wextra")
# add_definitions( -DDEBUG_VISUALIZATION )
set( CMAKE_BUILD_TYPE "DEBUG" )
# look for Cmake modules in project directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})

#########################################################
# FIND GLEW
#########################################################
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
#link_directories(${GLEW_LIBRARY_DIRS})



#########################################################
# FIND OPENAL
#########################################################
find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIRS})
link_directories(${OPENAL_LIBRARY_DIRS})
add_definitions(${OPENAL_DEFINITIONS})

#########################################################
# FIND ALUT
#########################################################
if(MINGW)
    find_package(ALUT) # MinGW does not find the headers; but for
                       # cross-compilation, we don't need a specific
                       # directory anyway
else(MINGW)
    find_package(ALUT REQUIRED)
endif(MINGW)
include_directories(${ALUT_INCLUDE_DIRS})
link_directories(${ALUT_LIBRARY_DIRS})
add_definitions(${ALUT_DEFINITIONS})

pkg_search_module(LIBEPOXY REQUIRED epoxy)
include_directories(${LIBEPOXY_INCLUDE_DIRS})

if (NOT TARGET_ARCHITECTURE)
    set(MARCH -march=native)
    set(MTUNE -mtune=native)
elseif ("${TARGET_ARCHITECTURE}" STREQUAL "generic")
    set(MARCH -march=x86-64)
    set(MTUNE -mtune=generic)
else ()
    set(MARCH -march=${TARGET_ARCHITECTURE})
    set(MTUNE -mtune=${TARGET_ARCHITECTURE})
endif ()

# cool dake stuff

ExternalProject_Add(
    dake
    GIT_REPOSITORY git://github.com/XanClic/dake.git
    GIT_TAG origin/master
    SOURCE_DIR dake
    CMAKE_COMMAND true
    BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
                  "CC=${CMAKE_C_COMPILER}"
                  "CXX=${CMAKE_CXX_COMPILER}"
                  "MARCH=${MARCH}"
                  "MTUNE=${MTUNE}"
                  libdake.a
    BINARY_DIR dake
    INSTALL_COMMAND true
)

ExternalProject_Get_Property(dake binary_dir)

include_directories(${binary_dir}/include)
link_directories(${binary_dir})

find_package(PNG REQUIRED)
link_directories(${PNG_LIBRARY_DIRS})

find_package(JPEG REQUIRED)
link_directories(${JPEG_LIBRARY_DIRS})

set(THREAD_CXXFLAGS -pthread)
set(THREAD_LIBS pthread)

#########################################################
# EXECUTABLE AUDIOphile
#########################################################

add_executable( audiophile src/main.cpp
                           # model
                           src/model/game.cpp
                           src/model/game_object.cpp
                           # controller
                           src/controller/engine.cpp
                           src/controller/glut_engine.cpp
                           src/controller/logic.cpp
                           # view
                           src/view/glut_window.cpp
                           src/view/gl_renderer.cpp
                           src/view/al_renderer.cpp
                           # flappy_box
                           src/flappy_box/model/box.cpp
                           src/flappy_box/model/explosion.cpp
                           src/flappy_box/model/game_over.cpp
                           src/flappy_box/model/paddle.cpp
                           src/flappy_box/model/power_up.cpp
                           src/flappy_box/model/world.cpp
                           src/flappy_box/controller/box_object_logic.cpp
                           src/flappy_box/controller/explosion_logic.cpp
                           src/flappy_box/controller/flappy_engine.cpp 
                           src/flappy_box/controller/paddle_logic.cpp
                           src/flappy_box/controller/power_up_logic.cpp
                           src/flappy_box/controller/world_logic.cpp
                           src/flappy_box/view/box_al_audible.cpp
                           src/flappy_box/view/box_gl_drawable.cpp
                           src/flappy_box/view/explosion_gl_drawable.cpp
                           src/flappy_box/view/explosion_al_audible.cpp
                           src/flappy_box/view/game_over_al_audible.cpp
                           src/flappy_box/view/game_over_gl_drawable.cpp
                           src/flappy_box/view/paddle_gl_drawable.cpp
                           src/flappy_box/view/paddle_al_audible.cpp
                           src/flappy_box/view/power_up_gl_drawable.cpp
                           src/flappy_box/view/world_al_audible.cpp
                           src/flappy_box/view/world_gl_drawable.cpp

                           # sound management
                           src/sound_provider.cpp
              )

target_link_libraries( audiophile -ldake ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${OPENAL_LIBRARY} ${ALUT_LIBRARY} ${GLEW_LIBRARIES} ${PNG_LIBRARY} ${JPEG_LIBRARY} txc_dxtn ${LIBEPOXY_LIBRARIES} ${THREAD_LIBS})
add_dependencies(audiophile dake)

set(CMAKE_CXX_FLAGS "-std=c++11 -O3 -g2 -Wall -Wextra -Wshadow \
                    ${MARCH} ${MTUNE} -fno-math-errno \
                    -fomit-frame-pointer -flto ${THREAD_CXXFLAGS}")


install(TARGETS audiophile RUNTIME DESTINATION bin)
