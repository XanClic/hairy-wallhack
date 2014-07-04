cmake_minimum_required( VERSION 2.8 )

project(audiophile)

include_directories( include )

add_definitions( "--std=c++11" )
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
                           src/flappy_box/controller/flappy_engine.cpp 
                           src/flappy_box/controller/box_object_logic.cpp 
                           src/flappy_box/view/box_al_audible.cpp
                           src/flappy_box/view/box_gl_drawable.cpp
                           
                           # sound management
                           src/sound_provider.cpp
              )

target_link_libraries( audiophile ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${OPENAL_LIBRARY} ${ALUT_LIBRARY} ${GLEW_LIBRARIES} )

install(TARGETS audiophile RUNTIME DESTINATION bin)