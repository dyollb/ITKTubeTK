##############################################################################
#
# Library:   TubeTK
#
# Copyright 2010 Kitware Inc. 28 Corporate Drive,
# Clifton Park, NY, 12065, USA.
#
# All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################

set( CMAKE_MODULE_PATH ${TubeTK_SOURCE_DIR}/CMake/Superbuild ${CMAKE_MODULE_PATH} )

set( base "${CMAKE_BINARY_DIR}" )
set_property( DIRECTORY PROPERTY EP_BASE ${base} )

if( DEFINED BUILD_SHARED_LIBS )
  set( shared ${BUILD_SHARED_LIBS} )
else( DEFINED BUILD_SHARED_LIBS )
  set( shared ON ) # use for BUILD_SHARED_LIBS on all subsequent projects
endif( DEFINED BUILD_SHARED_LIBS )
set( testing OFF ) # use for BUILD_TESTING on all subsequent projects
set( build_type "Release" )
if( CMAKE_BUILD_TYPE )
  set( build_type "${CMAKE_BUILD_TYPE}" )
endif( CMAKE_BUILD_TYPE )

set( gen "${CMAKE_GENERATOR}" )

# Find Git and determine proper protocol for accessing Git repositories. Users
# may need to choose HTTP is they are behind a firewall.
if( NOT GIT_EXECUTABLE )
  find_package( Git REQUIRED )
endif( NOT GIT_EXECUTABLE )

option( GIT_PROTOCOL_HTTP
  "Use HTTP for git access (useful if behind a firewall)" OFF )
mark_as_advanced( GIT_PROTOCOL_HTTP )

set( git_protocol "git" )
set( github_protocol "git" )
if( GIT_PROTOCOL_HTTP )
  set( git_protocol "http" )
  set( github_protocol "https" )
endif( GIT_PROTOCOL_HTTP )

include( ExternalProjectsConfig )

# Use for CMAKE_OSX_* in external projects.
set( CMAKE_OSX_EXTERNAL_PROJECT_ARGS )
if( APPLE )
  list( APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
    -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
    -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET} )
endif( APPLE )

#
# Add applications and modules build options
#
if( TubeTK_BUILD_APPLICATIONS )
  # Expose the TubeTK_BUILD_* variables to Superbuild
  file( GLOB_RECURSE TubeTK_modules_files
    "${TubeTK_SOURCE_DIR}/TubeTKModules.cmake" )
  foreach( f ${TubeTK_modules_files} )
    ExternalProject_Message( ${PROJECT_NAME} "Including ${f}" )
    include( ${f} )
  endforeach( f ${TubeTK_modules_files} )
endif( TubeTK_BUILD_APPLICATIONS )


macro(list_conditional_append cond list)
  if(${cond})
    list(APPEND ${list} ${ARGN})
  endif()
endmacro()

set( TubeTK_DEPENDENCIES "ITK" "SlicerExecutionModel" )

list_conditional_append( TubeTK_USE_VTK TubeTK_DEPENDENCIES "VTK" )

list_conditional_append( TubeTK_USE_CPPCHECK TubeTK_DEPENDENCIES "Cppcheck" )

list_conditional_append( TubeTK_USE_KWSTYLE TubeTK_DEPENDENCIES "KWStyle" )

list_conditional_append( TubeTK_USE_JSONCPP TubeTK_DEPENDENCIES "JsonCpp" )

list_conditional_append( TubeTK_USE_LIBSVM TubeTK_DEPENDENCIES "LIBSVM" )

list_conditional_append( TubeTK_USE_RANDOMFOREST TubeTK_DEPENDENCIES
  "RandomForest" )

set( TubeTK_EXTERNAL_PROJECTS_ARGS )

list_conditional_append( TubeTK_USE_BOOST
  TubeTK_EXTERNAL_PROJECTS_ARGS
    -DBOOST_ROOT=${BOOST_ROOT} )

list_conditional_append( TubeTK_USE_PYTHON
  TubeTK_EXTERNAL_PROJECTS_ARGS
    -DTubeTK_USE_NUMPY_STACK:BOOL=${TubeTK_USE_NUMPY_STACK}
    -DTubeTK_USE_PYQTGRAPH:BOOL=${TubeTK_USE_PYQTGRAPH}
    -DPYTHON_EXECUTABLE:FILEPATH=${PYTHON_EXECUTABLE}
    -DPYTHON_LIBRARY:FILEPATH=${PYTHON_LIBRARY}
    -DPYTHON_INCLUDE_DIR:PATH=${PYTHON_INCLUDE_DIR}
    -DWRAP_ITK_SWIG_ARGS_PYTHON:STRING=${WRAP_TubeTK_SWIG_ARGS_PYTHON} )

list_conditional_append( BUILD_TESTING_VIRTUAL_ENV
  TubeTK_EXTERNAL_PROJECTS_ARGS
    -DPythonVirtualEnvDir:PATH=${PythonVirtualEnvDir} )

# Append TubeTK_BUILD_MODULE_DEFAULT is has been specified by the user
if( DEFINED TubeTK_BUILD_MODULE_DEFAULT )
  list( APPEND TubeTK_EXTERNAL_PROJECTS_ARGS
    -DTubeTK_BUILD_MODULE_DEFAULT:BOOL=${TubeTK_BUILD_MODULE_DEFAULT} )
endif()

# Check dependencies.
set( EXTERNAL_PROJECT_DIR ${TubeTK_SOURCE_DIR}/CMake/Superbuild )

ExternalProject_Include_Dependencies(TubeTK
  DEPENDS_VAR TubeTK_DEPENDENCIES
  SUPERBUILD_VAR TubeTK_USE_SUPERBUILD )

if( CMAKE_VERSION VERSION_LESS 3.4 )
  set( CMAKE_CXX_COMPILER_LAUNCHER_FLAG )
  set( CMAKE_C_COMPILER_LAUNCHER_FLAG )
else()
  set( CMAKE_CXX_COMPILER_LAUNCHER_FLAG
    -DCMAKE_CXX_COMPILER_LAUNCHER:FILEPATH=${CMAKE_CXX_COMPILER_LAUNCHER} )
  set( CMAKE_C_COMPILER_LAUNCHER_FLAG
    -DCMAKE_C_COMPILER_LAUNCHER:FILEPATH=${CMAKE_C_COMPILER_LAUNCHER} )
endif()

# Normal build.
set( proj TubeTK )

ExternalProject_Add( ${proj}
  ${${proj}_EP_ARGS}
  DEPENDS ${TubeTK_DEPENDENCIES}
  DOWNLOAD_COMMAND ""
  DOWNLOAD_DIR ${TubeTK_SOURCE_DIR}
  SOURCE_DIR ${TubeTK_SOURCE_DIR}
  BINARY_DIR ${CMAKE_BINARY_DIR}/${proj}-build
  INSTALL_DIR ${CMAKE_BINARY_DIR}/${proj}-build
  CMAKE_GENERATOR ${gen}
  BUILD_AWAYS 1
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    ${CMAKE_CXX_COMPILER_LAUNCHER_FLAG}
    ${CMAKE_C_COMPILER_LAUNCHER_FLAG}
    -DCMAKE_EXE_LINKER_FLAGS:STRING=${CMAKE_EXE_LINKER_FLAGS}
    -DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
    ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
    -DBUILDNAME:STRING=${BUILDNAME}
    -DBUILD_DOCUMENTATION:BOOL=${BUILD_DOCUMENTATION}
    -DBUILD_SHARED_LIBS:BOOL=${shared}
    -DBUILD_TESTING:BOOL=${BUILD_TESTING}
    -DBUILD_TESTING_VIRTUAL_ENV:BOOL=${BUILD_TESTING_VIRTUAL_ENV}
    -DMAKECOMMAND:STRING=${MAKECOMMAND}
    -DGIT_EXECUTABLE:FILEPATH=${GIT_EXECUTABLE}
    -Dgit_protocol:STRING=${git_protocol}
    -Dgithub_protocol:STRING=${github_protocol}
    -DSITE:STRING=${SITE}
    -DTubeTK_BUILD_APPLICATIONS:BOOL=${TubeTK_BUILD_APPLICATIONS}
    -DTubeTK_CONFIG_BINARY_DIR:PATH=${TubeTK_CONFIG_BINARY_DIR}
    -DTubeTK_EXECUTABLE_DIRS:STRING=${TubeTK_EXECUTABLE_DIRS}
    -DTubeTK_SUPPORT_2D_IMAGES:BOOL=${TubeTK_SUPPORT_2D_IMAGES}
    -DTubeTK_USE_ARRAYFIRE:BOOL=${TubeTK_USE_ARRAYFIRE}
    -DTubeTK_USE_BOOST:BOOL=${TubeTK_USE_BOOST}
    -DTubeTK_USE_CPPCHECK:BOOL=${TubeTK_USE_CPPCHECK}
    -DTubeTK_USE_EXAMPLES_AS_TESTS:BOOL=${TubeTK_USE_EXAMPLES_AS_TESTS}
    -DTubeTK_USE_JSONCPP:BOOL=${TubeTK_USE_JSONCPP}
    -DTubeTK_USE_LIBSVM:BOOL=${TubeTK_USE_LIBSVM}
    -DTubeTK_USE_RANDOMFOREST:BOOL=${TubeTK_USE_RANDOMFOREST}
    -DTubeTK_USE_PYTHON:BOOL=${TubeTK_USE_PYTHON}
    -DTubeTK_USE_KWSTYLE:BOOL=${TubeTK_USE_KWSTYLE}
    -DTubeTK_USE_SUPERBUILD:BOOL=OFF
    -DTubeTK_USE_VTK:BOOL=${TubeTK_USE_VTK}
    ${TubeTK_EXTERNAL_PROJECTS_ARGS}
  INSTALL_COMMAND "" )
