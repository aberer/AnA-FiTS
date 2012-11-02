# $Id$

cmake_minimum_required (VERSION 2.6)
project (RandomLib)

set (RandomLib_VERSION_MAJOR 1)
set (RandomLib_VERSION_MINOR 6)
set (RandomLib_VERSION "${RandomLib_VERSION_MAJOR}.${RandomLib_VERSION_MINOR}")
set (PACKAGE_VERSION ${RandomLib_VERSION})
set (LIBVERSION 1)
set (LIBVERSIONFULL 1.0.6)

# Check for PACKAGE_PATH and INSTALL_PATH
#
#   If INSTALL_PATH is not specified but PACKAGE_PATH is, then
#   INSTALL_PATH is set to ${PACKAGE_PATH}/${PROJECT_NAME} (Windows) or
#   PACKAGE_PATH (non-Windows).
#
#   If INSTALL_PATH is now defined, then set CMAKE_INSTALL_PREFIX to
#   INSTALL_PATH.
if (NOT DEFINED INSTALL_PATH)
  if (DEFINED PACKAGE_PATH)
    if (WIN32)
      set (INSTALL_PATH ${PACKAGE_PATH}/${PROJECT_NAME}-${PACKAGE_VERSION}
	CACHE PATH "Installation Directory" FORCE)
    else ()
      set (INSTALL_PATH ${PACKAGE_PATH} CACHE PATH "Install Directory" FORCE)
    endif ()
  elseif (WIN32 AND DEFINED CMAKE_PREFIX_PATH
    AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # Backwards compatibility (support the mechanism advertised in
    # version 1.5): set CMAKE_INSTALL_PREFIX from first element of
    # CMAKE_PREFIX_PATH
    list (GET CMAKE_PREFIX_PATH 0 INSTALL_PATH)
    set (INSTALL_PATH ${INSTALL_PATH}/${PROJECT_NAME}-${PACKAGE_VERSION}
      CACHE PATH "Installation directory for GeographicLib" FORCE)
  endif ()
endif ()
if (DEFINED INSTALL_PATH)
  set (CMAKE_INSTALL_PREFIX ${INSTALL_PATH})
endif ()
message (STATUS "CMAKE_INSTALL_PREFIX set to ${CMAKE_INSTALL_PREFIX}")

# User-settable cache variables

# (1) Build as a shared library?  On Windows systems, this is typically
# more trouble than it's worth.
if (WIN32)
  option (RANDOM_SHARED_LIB "Build RandomLib as a shared library" OFF)
else ()
  option (RANDOM_SHARED_LIB "Build RandomLib as a shared library" ON)
endif ()

# (2) By default, cmake looks for hardware support of vector operations.
# This option allows you to disable this.
option (DISABLE_VECTOR_OPTIMIZATIONS "Do not look for SSE2 or AltiVec support"
  OFF)

# (3) By default, cmake looks for boost which is used by some of the
# examples. This option allows you to disable this.
option (DISABLE_BOOST "Do not look for boost libraries" OFF)

# Finding other packages.  (These are only need for the examples.  The
# library does not depend on them.)

# On Windows we use boost's static libraries.
if (NOT DISABLE_BOOST)
  if (WIN32)
    set (Boost_USE_STATIC_LIBS ON)
  endif ()
  find_package (Boost COMPONENTS serialization date_time)
endif ()

# Optionally use OpenMP in RandomParallel.
find_package (OpenMP)

# The debug version of the library is called Random_d.
set (CMAKE_DEBUG_POSTFIX _d)

# Look for vector support SSE2 for Intel chips and AltiVec for PowerPC.
include (CheckIncludeFileCXX)
if (DISABLE_VECTOR_OPTIMIZATIONS)
  set (VECTOR_FLAGS "")
  set (HAVE_SSE2 OFF)
  set (HAVE_ALTIVEC OFF)
else ()
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "i686" OR
    CMAKE_SYSTEM_PROCESSOR MATCHES "amd64" OR
    CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
    check_include_file_cxx ("emmintrin.h" HAVE_SSE2)
    if (HAVE_SSE2)
      if (WIN32)
        set (VECTOR_FLAGS "/arch:SSE2")
      else ()
        set (VECTOR_FLAGS "-msse2")
      endif ()
    else ()
      set (VECTOR_FLAGS "")
    endif ()
  elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "powerpc")
    check_type_size ("vector unsigned" VECTOR_UNSIGNED BUILTIN_TYPES_ONLY)
    if (HAVE_VECTOR_UNSIGNED)
      set (VECTOR_FLAGS "-maltivec")
      set (HAVE_ALTIVEC ON)
    else ()
      set (VECTOR_FLAGS "")
      set (HAVE_ALTIVEC OFF)
    endif ()
  endif ()
endif ()

# Optionally compile MPFR example.  This requires MPFR 3.0 or later.
check_include_file_cxx ("mpfr.h" HAVE_MPFR)
if (HAVE_MPFR)
  find_library (MPFR_LIBRARIES mpfr)
  find_library (GMP_LIBRARIES gmp)
  if (NOT (MPFR_LIBRARIES AND GMP_LIBRARIES))
    set (HAVE_MPFR  OFF)
  endif ()
endif ()

# Determine system properties
include (CheckTypeSize)
check_type_size ("long double" LONG_DOUBLE BUILTIN_TYPES_ONLY)

# Create a Config.h to expose system information to the compiler
configure_file (
    include/RandomLib/Config.h.in
    include/RandomLib/Config.h )

# The documentation depends on doxygen.  Because this require tools that
# may not be available on an end-user's system, the creation of the
# final documentation is therefore only done in "MAINTAINER" mode.  The
# maintainer runs "make distrib-all" which installs the transformed
# documentation files into the source tree.
if (NOT WIN32)
  set (DOXYGEN_SKIP_DOT ON)
  find_package (Doxygen)
endif ()
if (NOT WIN32 AND DOXYGEN_FOUND)
  set (MAINTAINER ON CACHE BOOL "Allow build of additional components")
else ()
  set (MAINTAINER OFF CACHE BOOL "Allow build of additional components")
endif ()
if (MAINTAINER)
  add_custom_target (distrib-all)
  add_dependencies (distrib-all distrib-doc)
endif ()

# Set a default build type for single-configuration cmake generators if
# no build type is set.
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   set (CMAKE_BUILD_TYPE Release)
endif ()

# On non-Windows machine, make the compiler more picky.  Also unrool
# loops in optimizing build types.
if (NOT WIN32)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VECTOR_FLAGS} -Wall -Wextra")
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -funroll-loops")
  set (CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -funroll-loops")
endif ()

# The list of examples to build.  Don't include MPFRExample here since
# it doesn't get linked with RandomLib.  This is treated specially in
# examples/CMakeLists.txt
set (EXAMPLES
  RandomExample RandomTime RandomThread RandomSave
  RandomExact RandomLambda RandomCoverage)

# Set the include directories.  Look in ${PROJECT_BINARY_DIR}/include
# first because that's where Config.h will be
include_directories ("${PROJECT_BINARY_DIR}/include")
include_directories (include)

# The list of subdirectories to process
add_subdirectory (src)
add_subdirectory (include/RandomLib)
add_subdirectory (examples)
add_subdirectory (doc)

# config file support for find_package(RandomLib).  This only supports
# finding RandomLib in the "install" tree (as opposed to the "build"
# tree).  The process installs randomlib-config.cmake in
# ${INSTALL_CMAKE_DIR} and @RANDOMLIB_ROOT_DIR@ is the relative path to
# the root from there.  (Thus the whole install tree can be relocated.)
if (WIN32)
  set (INSTALL_CMAKE_DIR "cmake")
  set (RANDOMLIB_ROOT_DIR "..")
else ()
  set (INSTALL_CMAKE_DIR
    "share/cmake/${PROJECT_NAME}-${PACKAGE_VERSION}")
  set (RANDOMLIB_ROOT_DIR "../../..")
endif ()
configure_file (cmake/randomlib-config.cmake.in
  cmake/randomlib-config.cmake @ONLY)
configure_file (cmake/randomlib-config-version.cmake.in
  cmake/randomlib-config-version.cmake @ONLY)
install (FILES
  "${PROJECT_BINARY_DIR}/cmake/randomlib-config.cmake"
  "${PROJECT_BINARY_DIR}/cmake/randomlib-config-version.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}")

# Make information about the cmake targets (the library) available.
install (EXPORT depends
  FILE randomlib-depends.cmake
  DESTINATION "${INSTALL_CMAKE_DIR}")

# Packaging support; we deal with 
# (1) a source distribution: cmake make a tar.gz file and the zip file
# is created from this.  Only the maintainer can do this, because of
# the need to generate additional documentation files.
# (2) a binary distribution: code is included for Linux, Apple, and
# Windows, but only the Windows distribution has been exercised.

# Need to ensure that system dlls get included in a binary distribution
if (NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
  # Visual Studio Express does include redistributable components so
  # squelch the warning.
  set (CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
endif ()
include (InstallRequiredSystemLibraries)

# The configuration of CPack is via variable that need to be set before
# the include (CPack).
set (CPACK_PACKAGE_VERSION_MAJOR ${RandomLib_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${RandomLib_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH 0)
set (CPACK_PACKAGE_CONTACT charles@karney.com)
set (CPACK_PACKAGE_VENDOR "RandomLib")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY
  "Random library and documentation")
# The list of files to be excluded from the source distribution.
set (CPACK_SOURCE_IGNORE_FILES
  "#"
  "~\$"
  "/\\\\.git"
  "${PROJECT_SOURCE_DIR}/BUILD"
  "${PROJECT_SOURCE_DIR}/distrib/"
  "${PROJECT_SOURCE_DIR}/[^/]*\\\\.html\$"
  "${PROJECT_SOURCE_DIR}/makefile-admin\$" )
set (CPACK_SOURCE_GENERATOR TGZ)

set (CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE.txt)
set (CPACK_PACKAGE_INSTALL_DIRECTORY
  "${PROJECT_NAME}-${PACKAGE_VERSION}")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")

if (WIN32)
  # The Windows binary packager is NSIS.  Set the necessary variables
  # for this.
  set (CPACK_NSIS_CONTACT "charles@karney.com")
  set (CPACK_NSIS_URL_INFO_ABOUT "http://randomlib.sf.net")
  set (CPACK_NSIS_HELP_LINK "mailto:charles@karney.com")
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    # Hardcode the prefix for Visual Studio 10
    set (CPACK_NSIS_INSTALL_ROOT "C:\\\\pkg-vc10-x64")
    set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}-win64")
    set (CPACK_NSIS_PACKAGE_NAME
      "${PROJECT_NAME} x64 ${PACKAGE_VERSION}")
    set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY
      "${PROJECT_NAME}-x64-${PACKAGE_VERSION}")
  else ()
    # Hardcode the prefix for Visual Studio 10
    set (CPACK_NSIS_INSTALL_ROOT "C:\\\\pkg-vc10")
    set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}-win32")
    set (CPACK_NSIS_PACKAGE_NAME
      "${PROJECT_NAME} ${PACKAGE_VERSION}")
    set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY
      "${PROJECT_NAME}-${PACKAGE_VERSION}")
  endif ()
  set (CPACK_NSIS_DISPLAY_NAME ${CPACK_NSIS_PACKAGE_NAME})
  set (CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
  CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Library Documentation.lnk\\\" \\\"$INSTDIR\\\\share\\\\doc\\\\RandomLib\\\\html\\\\index.html\\\"
")
  set (CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
  Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\Library Documentation.lnk\\\"
")
  set (CPACK_NSIS_MODIFY_PATH ON)
elseif (APPLE)
  # Not tested
  set (CPACK_GENERATOR DMG)
  set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}-darwin")
else ()
  # Not tested
  set (CPACK_GENERATOR TGZ)
endif ()

include (CPack)

# Another maintainer-specific target is building the source distribution
# via the target dist.  This calls package_source to make a tar.gz file.
# However this needs to be touched up to support the vanilla Makefiles
# provided with RandomLib.  This entails
# (1) creating Makefile (which includes Makefile.mk);
# (2) creating a bare-bones Config.h (with just the version information);
# (3) making sure that make thinks the generated documentation files are
# up-to-date.
# Then a new tar.gz file and zip file are created.  To avoid potential
# problems with directory permissions, tar and zip are told only to
# archive the files.
if (MAINTAINER)
  add_custom_target (dist
    COMMAND ${CMAKE_MAKE_PROGRAM} package_source
    COMMAND
      cd _CPack_Packages/Linux-Source/TGZ/${CPACK_SOURCE_PACKAGE_FILE_NAME} &&
      echo include Makefile.mk > Makefile &&
      sed -e "s/Unconfigured/${PACKAGE_VERSION}/"
      ${PROJECT_SOURCE_DIR}/include/RandomLib/Config.h.template
      > include/RandomLib/Config.h
    COMMAND
      cd _CPack_Packages/Linux-Source/TGZ/${CPACK_SOURCE_PACKAGE_FILE_NAME} &&
      touch doc/html/index.html
    COMMAND
      cd _CPack_Packages/Linux-Source/TGZ &&
      find ${CPACK_SOURCE_PACKAGE_FILE_NAME} -type f |
      tar cfzT ${CMAKE_BINARY_DIR}/${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.gz -
    COMMAND
      rm -f ${CPACK_SOURCE_PACKAGE_FILE_NAME}.zip &&
      cd _CPack_Packages/Linux-Source/TGZ &&
      find ${CPACK_SOURCE_PACKAGE_FILE_NAME} -type f |
      zip -q ${CMAKE_BINARY_DIR}/${CPACK_SOURCE_PACKAGE_FILE_NAME}.zip -@
  )
  add_dependencies (dist distrib-all)
endif ()

# Add a test target; the tests are in tools.
enable_testing ()
