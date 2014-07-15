cmake_minimum_required( VERSION 2.8 )

project(audiophile)

include(ExternalProject)

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
 find_package(ALUT REQUIRED)
 include_directories(${ALUT_INCLUDE_DIRS})
 link_directories(${ALUT_LIBRARY_DIRS})
 add_definitions(${ALUT_DEFINITIONS})

# cool dake stuff

ExternalProject_Add(
    dake
    GIT_REPOSITORY git://github.com/XanClic/dake.git
    GIT_TAG origin/master
    SOURCE_DIR dake
    CMAKE_COMMAND true
    BUILD_COMMAND make
    BINARY_DIR dake
    INSTALL_COMMAND true
)

ExternalProject_Get_Property(dake binary_dir)

include_directories(${binary_dir}/include)
link_directories(${binary_dir})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})
link_directories(${PNG_LIBRARY_DIRS})
add_definitions(${PNG_DEFINITIONS})

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

                           src/resource_finder.cpp
              )

target_link_libraries( audiophile -ldake ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${OPENAL_LIBRARY} ${ALUT_LIBRARY} ${GLEW_LIBRARIES} ${PNG_LIBRARY} )
add_dependencies(audiophile dake)

install(TARGETS audiophile RUNTIME DESTINATION bin)
