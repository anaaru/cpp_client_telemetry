#cmake_minimum_required(VERSION 3.0)

#set(MAT_SDK_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../lib/include/public")
#set(MAT_SDK_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")

set(CMAKE_PROJECT_NAME "MAT-sdk")
set(CPACK_SYSTEM_NAME "${OS_RELEASE_ID}-${OS_RELEASE_VERSION_ID}")
#set(CPACK_SET_DESTDIR ${CPACK_SYSTEM_NAME})
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
set(CPACK_GENERATOR "DEB")

set(MAJOR_VERSION "3")
set(MINOR_VERSION "2")
string(TIMESTAMP DAYNUMBER "%j")
set(PATCH_VERSION "${DAYNUMBER}")

set(CPACK_PACKAGE_DESCRIPTION "Microsoft Applications Telemetry SDK for Linux")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Microsoft Applications Telemetry SDK for events ingestion from Linux hosts")
set(CPACK_PACKAGE_VENDOR "Microsoft")
set(CPACK_PACKAGE_CONTACT "1ds.sdk.cpp@service.microsoft.com")
set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")

# FIXME: add architecture name in file name
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}-${CPACK_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
message("-- Package name: ${CPACK_PACKAGE_FILE_NAME}.deb")

#install(TARGETS ${MAT_SDK_LIB_DIR}/libMAT.a  ARCHIVE DESTINATION lib/MAT COMPONENT headers)
#install(FILES ${MAT_SDK_INC_DIR}/*.* DESTINATION include/MAT COMPONENT libraries)

#set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS " kate , plasma-desktop, libc6 (>= 2.1.3) ")
#set(CPACK_DEBIAN_PACKAGE_DEPENDS " kate , dolphin ")
#set(CPACK_DEBIAN_PACKAGE_SECTION "kde")

set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
#set(CPACK_COMPONENTS_ALL headers libraries)

include(CPack)
