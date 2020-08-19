cmake_minimum_required( VERSION 2.8 )
set (CMAKE_CXX_STANDARD 11)
project( new_version )

# Create a list with all .cpp source files & .h header files
set( project_sources
    main.cpp
    model_obj.cpp
    model_obj.h
    shader.h
    stb_image.h
    texture.cpp
    object.h
    catmullrom.h
    catmullrom3.h
)

# Adding the exec
add_executable(${PROJECT_NAME} ${project_sources})

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

# Glfw
find_package( glfw3 REQUIRED )
target_link_libraries(${PROJECT_NAME} glfw)

# Glew
find_package( GLEW 2.0 REQUIRED )
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})

#glm
find_package(GLM REQUIRED)
target_link_libraries(${PROJECT_NAME} ${GLM_LIBRARIES})


# Copy relevant files to build folder (source destination name)


configure_file(spirit/spirit.obj ${CMAKE_BUILD_RPATH} spirit/spirit.obj)
configure_file(spirit/spirit.mtl ${CMAKE_BUILD_RPATH} spirit/spirit.mtl)
configure_file(spirit/spirit.png  ${CMAKE_BUILD_RPATH} spirit/spirit.png COPYONLY)

configure_file(rover/rover.obj ${CMAKE_BUILD_RPATH} rover/rover.obj)
configure_file(rover/rover.mtl ${CMAKE_BUILD_RPATH} rover/rover.mtl)
configure_file(rover/rover.png  ${CMAKE_BUILD_RPATH} rover/rover.png COPYONLY)

configure_file(rover/rover14Arm.obj ${CMAKE_BUILD_RPATH} rover/rover14Arm.obj)
configure_file(rover/rover14Arm.mtl ${CMAKE_BUILD_RPATH} rover/rover14Arm.mtl)
configure_file(rover/armtexture14.png  ${CMAKE_BUILD_RPATH} rover/armtexture14.png COPYONLY)

configure_file(rover/rover15Body.obj ${CMAKE_BUILD_RPATH} rover/rover15Body.obj)
configure_file(rover/rover15Body.mtl ${CMAKE_BUILD_RPATH} rover/rover15Body.mtl)
configure_file(rover/bodyTexture14.png  ${CMAKE_BUILD_RPATH} rover/bodyTexture14.png COPYONLY)

configure_file(skysphere/SkyboxBackdrop3.obj ${CMAKE_BUILD_RPATH} skysphere/SkyboxBackdrop3.obj)
configure_file(skysphere/SkyboxBackdrop3.mtl ${CMAKE_BUILD_RPATH} skysphere/SkyboxBackdrop3.mtl)
configure_file(skysphere/SkyboxBackdrop3.png  ${CMAKE_BUILD_RPATH} skysphere/SkyboxBackdrop3.png COPYONLY)

configure_file(res/mainShader.shader ${CMAKE_BUILD_RPATH} res/mainShader.shader)
configure_file(res/frag.shader ${CMAKE_BUILD_RPATH} res/frag.shader)
configure_file(res/vert.shader ${CMAKE_BUILD_RPATH} res/vert.shader)
configure_file(res/cartoon.fs ${CMAKE_BUILD_RPATH} res/cartoon.fs)
configure_file(res/cartoon.vs ${CMAKE_BUILD_RPATH} res/cartoon.vs)

#configure_file("rover_diffuse.png" ${CMAKE_BUILD_RPATH} "rover_diffuse.png")
