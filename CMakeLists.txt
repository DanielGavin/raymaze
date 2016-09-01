cmake_minimum_required(VERSION 2.6)
project(raymaze)


if (WIN32 AND CMAKE_SYSTEM_VERSION)
    set( CMAKE_CXX_FLAGS "-W4 -EHsc" )
    set_property( GLOBAL PROPERTY USE_FOLDERS ON)	
else() 
    set( CMAKE_CXX_FLAGS "-std=c++11" )
endif()


MACRO(SOURCE_GROUP_BY_FOLDER target)
  SET(SOURCE_GROUP_DELIMITER "/")
  SET(last_dir "")
  SET(files "")

  FOREACH(file ${SOURCE} ${HEADER})
    file(RELATIVE_PATH relative_file ${target} ${file})
    GET_FILENAME_COMPONENT(dir "${relative_file}" DIRECTORY)
    #MESSAGE(${file})
    IF (NOT "${dir}" STREQUAL "${last_dir}")
      IF (files)
        SOURCE_GROUP("${last_dir}" FILES ${files})
      ENDIF (files)
      SET(files "")
    ENDIF (NOT "${dir}" STREQUAL "${last_dir}")
    SET(files ${files} ${file})
    SET(last_dir "${dir}")
  ENDFOREACH(file)
  IF (files)
    SOURCE_GROUP("${last_dir}" FILES ${files})
  ENDIF (files)
ENDMACRO(SOURCE_GROUP_BY_FOLDER)


add_executable(raymaze raymaze.c raycast_render.h raycast_render.c)

