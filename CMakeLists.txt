cmake_minimum_required(VERSION 2.8)

project(ldapjpeg)
#set(EXECUTABLE_OUTPUT_PATH bin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")
find_package(PkgConfig REQUIRED)

find_package(Pam REQUIRED)
include_directories(${PAM_INCLUDE_DIR})

pkg_check_modules(DBUS dbus-1)
include_directories(${DBUS_INCLUDE_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})
add_definitions(${DBUS_CFLAGS_OTHER})

find_package(Ldap REQUIRED)
include_directories(${LDAP_INCLUDE_DIRS})

add_executable(getldapjpeg
	src/main.c
	src/err.c
	src/ldap-client.c
	src/dbus-client.c
)

target_link_libraries(getldapjpeg
	${DBUS_LIBRARIES}
	${LDAP_LIBRARIES}
)

add_library(pam_ldapjpeg SHARED
	src/pam-module.c
	src/ldap-client.c
	src/dbus-client.c
)

target_link_libraries(pam_ldapjpeg
	${PAM_LIBRARIES}
	${DBUS_LIBRARIES}
	${LDAP_LIBRARIES}
)
