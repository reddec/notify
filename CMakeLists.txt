cmake_minimum_required(VERSION 2.8.4)
project(nottingham)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic")

add_library(notify SHARED notify.c)
add_library(notify_static STATIC notify.c)
set_target_properties(notify_static PROPERTIES OUTPUT_NAME notify)

install(FILES notify.h  DESTINATION "/usr/local/include/notify.h")
install(TARGETS notify  DESTINATION "/usr/local/lib/")
install(TARGETS notify_static  DESTINATION "/usr/local/lib/")

# Setup DEBIAN control files
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_DESCRIPTION "Multicast notification. Source - https://github.com/reddec/notify")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Red Dec <net.dev@mail.ru>")

include(CPack)