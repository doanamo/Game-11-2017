# Minimum version required.
CMake_Minimum_Required(VERSION 3.9.5)

#
# Config
#

# Build settings.
Set(ProjectName "Project")
Set(TargetName "Game")

# Application settings.
Set(WorkingDir "../Deploy")
Set(ShowConsole ON)

#
# Source
#

# Source files.
Set(SourceDir "../Source")
Set(PrecompiledHeader "Precompiled.hpp")
Set(PrecompiledSource "Precompiled.cpp")

Set(SourceFiles
    "Main.cpp"
    
    "${PrecompiledHeader}"
    "${PrecompiledSource}"
    
    "Common/Debug.hpp"
    "Common/Build.hpp"
    "Common/Build.cpp"
    "Common/Utility.hpp"
    "Common/Utility.cpp"
    "Common/Noncopyable.hpp"
    "Common/ScopeGuard.hpp"
    "Common/Delegate.hpp"
    "Common/Receiver.hpp"
    "Common/Dispatcher.hpp"
    "Common/Collector.hpp"

    "Logger/Logger.hpp"
    "Logger/Logger.cpp"
    "Logger/Message.hpp"
    "Logger/Message.cpp"
    "Logger/Output.hpp"
    "Logger/Sink.hpp"
    "Logger/Sink.cpp"
    "Logger/Outputs/DebuggerOutput.hpp"
    "Logger/Outputs/DebuggerOutput.cpp"
    "Logger/Outputs/ConsoleOutput.hpp"
    "Logger/Outputs/ConsoleOutput.cpp"
    "Logger/Outputs/FileOutput.hpp"
    "Logger/Outputs/FileOutput.cpp"

    "System/Context.hpp"
    "System/Context.cpp"
    "System/Config.hpp"
    "System/Config.cpp"
    "System/Timer.hpp"
    "System/Timer.cpp"
    "System/Window.hpp"
    "System/Window.cpp"
    "System/InputState.hpp"
    "System/InputState.cpp"
    "System/ResourcePool.hpp"
    "System/ResourceManager.hpp"
    "System/ResourceManager.cpp"

    "Graphics/Buffer.hpp"
    "Graphics/Buffer.cpp"
    "Graphics/VertexInput.hpp"
    "Graphics/VertexInput.cpp"
    "Graphics/Shader.hpp"
    "Graphics/Shader.cpp"
    "Graphics/Sampler.hpp"
    "Graphics/Sampler.cpp"
    "Graphics/Texture.hpp"
    "Graphics/Texture.cpp"
    "Graphics/ScreenSpace.hpp"
    "Graphics/ScreenSpace.cpp"
    "Graphics/Sprite.hpp"
    "Graphics/Sprite.cpp"
    "Graphics/BasicRenderer.hpp"
    "Graphics/BasicRenderer.cpp"
)

# Append source directory path to each source file.
Message("-- Appending directory path to each source file...")

Set(SourceFilesTemp)

ForEach(SourceFile ${SourceFiles})
    List(APPEND SourceFilesTemp "${SourceDir}/${SourceFile}")
    Message("SourceFile: ${SourceFile} => ${SourceDir}/${SourceFile}")
EndForEach()

Set(SourceFiles ${SourceFilesTemp})

# Organize source files based on their directory structure.
Message("-- Organizing source files by their directories...")

ForEach(SourceFile ${SourceFiles})
    # Get the relative path to source file's directory.
    Get_Filename_Component(SourceFilePath ${SourceFile} PATH)
    
    # Remove the path to base source directory.
    String(REPLACE "${SourceDir}" "" SourceFilePath ${SourceFilePath})
    
    # Add a source file to a group.
    If(NOT ${SourceFilePath} STREQUAL "")
        # Normalize path separators as required by e.g. Visual Studio.
        String(REPLACE "/" "\\" SourceFilePath ${SourceFilePath})
        
        # Remove the leading path separator.
        String(SUBSTRING ${SourceFilePath} 0 1 FirstCharacter)
        
        If(FirstCharacter STREQUAL "\\")
            String(LENGTH ${SourceFilePath} SourceFilePathLength)
            Math(EXPR SourceFilePathLength "${SourceFilePathLength} - 1")
            String(SUBSTRING ${SourceFilePath} 1 ${SourceFilePathLength} SourceFilePath)
        EndIf()
        
        # Add a source file to a corresponding group.
        Source_Group("Source\\${SourceFilePath}" FILES ${SourceFile})
    Else()
        # Add a source file to the root group.
        Source_Group("Source" FILES ${SourceFile})
    EndIf()
    
    Message("SourceFilePath: ${SourceFile} => ${SourceFilePath}")
EndForEach()

#
# Build
#

# Create the project.
Project(${ProjectName})

# Create an executable target.
Add_Executable(${TargetName} ${SourceFiles})

# Add the source directory as an include directory.
Include_Directories(${SourceDir})

# Enable unicode support.
Add_Definitions(-DUNICODE -D_UNICODE)

# Enable target folders.
Set_Property(GLOBAL PROPERTY USE_FOLDERS ON)

# Move predefined targets to a separate folder.
Set_Property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

#
# Debug
#

# Write a file in the build directory that will point to the working directory.
Get_Filename_Component(WorkingDirPath "${CMAKE_CURRENT_LIST_DIR}/${WorkingDir}" ABSOLUTE)
File(WRITE "${CMAKE_BINARY_DIR}/WorkingDir.txt" "${WorkingDirPath}/")

# Write a file in the build directory that will point to the source directory.
Get_Filename_Component(SourceDirPath "${CMAKE_CURRENT_LIST_DIR}/${SourceDir}" ABSOLUTE)
File(WRITE "${CMAKE_BINARY_DIR}/SourceDir.txt" "${SourceDirPath}/")

#
# Platform
#

# Enable C++17 for all supported compilers.
Set(CMAKE_CXX_STANDARD 17)
Set(CMAKE_CXX_STANDARD_REQUIRED ON)
Set(CMAKE_CXX_EXTENSIONS OFF)

# MSVC compiler.
If("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # Show or hide the console window.
    If(ShowConsole)
        Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/SUBSYSTEM:Console ")
    Else()
        Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/SUBSYSTEM:Windows ")
    EndIf()
    
    # Restore default main() entry instead of WinMain().
    Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY LINK_FLAGS "/ENTRY:mainCRTStartup ")

    # Enable C++17 support by default.
    # CMake flags for C++17 do not work with MSVC compiler as of now.
    Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    
    # Disable Standard C++ Library warnings.
    Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
    Set_Property(TARGET ${TargetName} APPEND_STRING PROPERTY COMPILE_DEFINITIONS "_SCL_SECURE_NO_WARNINGS")
    
    # Use the precompiled header.
    Get_Filename_Component(PrecompiledName ${PrecompiledHeader} NAME_WE)
    
    Set(PrecompiledBinary "$(IntDir)/${PrecompiledName}.pch")
    
    Set_Source_Files_Properties(${SourceFiles} PROPERTIES 
        COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
        OBJECT_DEPENDS "${PrecompiledBinary}"
    )
    
    Set_Source_Files_Properties("${SourceDir}/${PrecompiledSource}" PROPERTIES 
        COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
        OBJECT_OUTPUTS "${PrecompiledBinary}"
    )
EndIf()

# GCC compiler.
If("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Enable C++17 support by default.
    # List(APPEND CMAKE_CXX_FLAGS "-std=c++17")
EndIf()

#
# GLM
#

# Add include directory.
Include_Directories("../External/GLM-0.9.8.5")

#
# OpenGL
#

# Find library.
Find_Package(OpenGL REQUIRED)

# Link library.
Target_Link_Libraries(${TargetName} ${OPENGL_gl_LIBRARY})

#
# GLEW
#

# Set build options.
Set(BUILD_UTILS OFF)

# Add library directory.
Add_Subdirectory("../External/GLEW-2.1.0/build/cmake" "GLEW" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/GLEW-2.1.0/include")

# Move library target to dependencies.
Set_Property(TARGET "glew_s" PROPERTY PROJECT_LABEL "GLEW")
Set_Property(TARGET "glew_s" PROPERTY FOLDER "External")

# Link library target.
Add_Dependencies(${TargetName} "glew_s")
Target_Link_Libraries(${TargetName} "glew_s")

#
# GLFW
#

# Set build options.
Set(GLFW_BUILD_EXAMPLES OFF)
Set(GLFW_BUILD_TESTS OFF)
Set(GLFW_BUILD_DOCS OFF)
Set(GLFW_INSTALL OFF)

# Add library directory.
Add_Subdirectory("../External/GLFW-3.2.1" "GLFW" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/GLFW-3.2.1/include")

# Set target properties.
Set_Property(TARGET "glfw" PROPERTY PROJECT_LABEL "GLFW")
Set_Property(TARGET "glfw" PROPERTY FOLDER "External")

# Link library target.
Add_Dependencies(${TargetName} "glfw")
Target_Link_Libraries(${TargetName} "glfw")

#
# ZLib
#

# Set build options.
Set(SKIP_INSTALL_ALL ON)

# Add library directory.
Add_Subdirectory("../External/ZLib-1.2.11" "ZLib" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/ZLib-1.2.11")
Include_Directories("${CMAKE_BINARY_DIR}/ZLib")

# Move library target to dependencies.
Set_Property(TARGET "zlibstatic" PROPERTY PROJECT_LABEL "ZLib")
Set_Property(TARGET "zlibstatic" PROPERTY FOLDER "External")

# Link library target.
Add_Dependencies(${TargetName} "zlibstatic")
Target_Link_Libraries(${TargetName} "zlibstatic")

# Help dependencies find this library.
Set(ZLIB_ROOT "../External/ZLib-1.2.11")
Set(ZLIB_INCLUDE_DIR "../External/ZLib-1.2.11")
Set(ZLIB_LIBRARY "zlibstatic")

#
# PNG
#

# Set build options.
Set(PNG_SHARED OFF)
Set(PNG_STATIC ON)
Set(PNG_TESTS OFF)
Set(SKIP_INSTALL_ALL ON)

# Add library directory.
Add_Subdirectory("../External/PNG-1.6.34" "PNG" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/PNG-1.6.34")
Include_Directories("${CMAKE_BINARY_DIR}/PNG")

# Move library target to dependencies.
Set_Property(TARGET "png_static" PROPERTY PROJECT_LABEL "PNG")
Set_Property(TARGET "png_static" PROPERTY FOLDER "External")

Set_Property(TARGET "genfiles" PROPERTY PROJECT_LABEL "PNG_Config")
Set_Property(TARGET "genfiles" PROPERTY FOLDER "External")

# Link library target.
Add_Dependencies(${TargetName} "png_static")
Target_Link_Libraries(${TargetName} "png_static")

#
# Lua
#

# Add library directory.
Add_Subdirectory("../External/Lua-5.3.4" "Lua" EXCLUDE_FROM_ALL)

# Add include directory.
Include_Directories("../External/Lua-5.3.4/src")

# Move library target to dependencies.
Set_Property(TARGET "lua_static" PROPERTY PROJECT_LABEL "Lua")
Set_Property(TARGET "lua_static" PROPERTY FOLDER "External")

# Link library target.
Add_Dependencies(${TargetName} "lua_static")
Target_Link_Libraries(${TargetName} "lua_static")
