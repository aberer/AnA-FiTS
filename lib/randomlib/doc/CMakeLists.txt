# $Id$

# Runs doxygen, if available

# First assemble a list of all the files the documentation uses.  Add a
# dependency on htmlman (from man/CMakeLists.txt).  Use doxygen.log as
# the make target.  To make this target, copy the non-doxygen generated
# files into html/.  Run doxfile.in thru cmake's config process so that
# absolute path names are used and so that the pathnames are properly
# stripped by doxygen (via STRIP_FROM_PATH). The distrib-doc target
# copies the html directory into the source tree.

# If doxygen is not available, only the install step (from the source
# tree) is done.

if (DOXYGEN_FOUND)
  configure_file (
      "${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in"
      "${CMAKE_CURRENT_BINARY_DIR}/doxyfile" )
  file (GLOB SOURCES
    ../src/*.cpp ../include/RandomLib/*.hpp ../examples/*.cpp)
  file (GLOB EXTRA_FILES *hist.png *hist.pdf ../LICENSE.txt)
  add_custom_target (doc ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxygen.log)
  add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doxygen.log
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxyfile
      Random.doc ${SOURCES} ${EXTRA_FILES}
    COMMAND rm -rf html && mkdir html
    COMMAND
      cp -p ${EXTRA_FILES} html/
    COMMAND ${DOXYGEN_EXECUTABLE} doxyfile > doxygen.log
    COMMENT "Generating html documentation tree")
  install (DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
    DESTINATION share/doc/RandomLib)

  add_custom_target (distrib-doc)
  add_dependencies (distrib-doc doc)
  add_custom_command (TARGET distrib-doc
    COMMAND rsync -a --delete html/ ${CMAKE_CURRENT_SOURCE_DIR}/html/
    COMMAND touch ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html
    COMMENT "Installing html documentation tree in source tree")

else ()
  install (DIRECTORY html DESTINATION share/doc/RandomLib)
endif ()
