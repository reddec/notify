cmake_minimum_required(VERSION 2.8.4)
project(nottingham)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")

add_custom_target(man ALL)

add_custom_command(
  TARGET man
  SOURCE src/notify.h doxygen/Doxyfile
  WORKING_DIRECTORY doxygen
  COMMAND doxygen Doxyfile
  OUTPUTS ${CMAKE_CURRENT_SOURCE_DIR}/doc/man/man3/notify.h.3
)

add_custom_command(
  TARGET man
  SOURCE man
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/notify.h ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile
)

add_library(notify SHARED src/notify.c)
add_library(notify_static STATIC src/notify.c)
set_target_properties(notify_static PROPERTIES OUTPUT_NAME notify)

install(FILES src/notify.h  DESTINATION "/usr/local/include/")
install(TARGETS notify  DESTINATION "/usr/local/lib/")
install(TARGETS notify_static  DESTINATION "/usr/local/lib/")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc/man/man3/ DESTINATION "/usr/local/share/man/man3/" FILES_MATCHING PATTERN notify* )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc/man/man3/ DESTINATION "/usr/local/share/man/man3/" FILES_MATCHING PATTERN __NOTIFY* )

# Setup DEBIAN control files
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_DESCRIPTION "Multicast notification. Source - https://github.com/reddec/notify")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Red Dec <net.dev@mail.ru>")

include(CPack)