CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(drm-service-core-tizen CXX)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(LIBDIR "\${prefix}/lib")
SET(INCLUDEDIR "\${prefix}/include")

SET(DRMTESTUTIL drm_test_tizen)
SET(DRMTAPPSINITIALIZE drm_tizen_initialize)

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    ADD_DEFINITIONS("-DTIZEN_DEBUG_ENABLE")
    ADD_DEFINITIONS("-DBUILD_TYPE_DEBUG")
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

INCLUDE(FindPkgConfig)

ADD_DEFINITIONS("-DISSUER_ROOT_CERTS_DIR=\"${TZ_SYS_SHARE}/tizen_app_drm/root_certs\"")
ADD_DEFINITIONS("-DDB_PATH=\"${TZ_SYS_DB}/.dtapps.db\"")


############################# compiler flags ##################################
SET(CMAKE_C_FLAGS_PROFILING    "-g -O0 -pg -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_PROFILING  "-g -std=c++0x -O0 -pg -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_C_FLAGS_DEBUG        "-g -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_CXX_FLAGS_DEBUG      "-g -std=c++0x -O0 -ggdb -Wp,-U_FORTIFY_SOURCE")
SET(CMAKE_C_FLAGS_RELEASE      "-g -O2")
SET(CMAKE_CXX_FLAGS_RELEASE    "-g -std=c++0x -O2")
SET(CMAKE_C_FLAGS_CCOV         "-g -O2 --coverage")
SET(CMAKE_CXX_FLAGS_CCOV       "-g -std=c++0x -O2 --coverage")

ADD_DEFINITIONS("-fPIC")

# Set compiler warning flags
ADD_DEFINITIONS("-Werror")                      # Make all warnings into errors.
ADD_DEFINITIONS("-Wall")                        # Generate all warnings
ADD_DEFINITIONS("-Wextra")                      # Generate even more extra warnings

INCLUDE_DIRECTORIES(
    SYSTEM
    ${CMAKE_CURRENT_SOURCE_DIR}/tadcore/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tappsd/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

pkg_check_modules(DRM_SERVICE_CORE_DEP
    REQUIRED
	dlog
	db-util
	openssl
    cryptsvc
    libtzplatform-config
)

INCLUDE_DIRECTORIES(${DRM_SERVICE_CORE_DEP_INCLUDE_DIRS})

SET(SRCS
	tadcore/TADCCore/TADC_Core.cpp
	tadcore/TADCCore/TADC_Sub.cpp
	tadcore/TADCCore/TADC_Util.cpp
	tadcore/TADCInterface/TADC_IF.cpp
	tadcore/Svc/DrmTdcSvc.cpp
	tappsd/src/intf/drm_intf_tapps.cpp
	tadcore/XMLParser/CPointerArray.cpp
	tadcore/XMLParser/CXMLAttribute.cpp
	tadcore/XMLParser/CXMLElement.cpp
	tadcore/XMLParser/CXMLFile.cpp
	tappsd/src/rights/DTapps2Rights.cpp
	tappsd/src/db/DTapps2SqlData.cpp
	tappsd/src/db/DTapps2SqliteDB.cpp
	tappsd/src/util/DTapps2Base64.cpp
	tappsd/src/util/DTapps2HMAC.cpp
	tappsd/src/util/DTapps2Time.cpp
	service/drm-tapps.cpp
	tadcore/DrmFileHandleMgr/DrmFileHandler.cpp
	tadcore/DrmFileHandleMgr/DrmFileMgr.cpp
	tadcore/DrmFileHandleMgr/DrmFileApi.cpp

	#for deviceId
	tadcore/TADCInterface/DUIDGenerator.cpp
)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})

SET_TARGET_PROPERTIES(
    ${PROJECT_NAME}
    PROPERTIES
        COMPILE_FLAGS " -fvisibility=hidden"
        SOVERSION ${MAJORVER}
        VERSION ${FULLVER}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ${DRM_SERVICE_CORE_DEP_LIBRARIES}
    -ldevice_info # libdevice_info.so in cryptsvc.
)

#################################################################
pkg_check_modules(DRM_INITIALIZE_DEP
    REQUIRED
	dlog
	db-util
	openssl
#    cryptsvc
)

INCLUDE_DIRECTORIES(${DRM_INITIALIZE_DEP_INCLUDE_DIRS})

SET(DRMTAPPSINITIALIZE_SRCS
	service/drm-tapps-initialize.cpp
	tappsd/src/intf/drm_intf_tapps.cpp
	tappsd/src/db/DTapps2SqliteDB.cpp
	tappsd/src/db/DTapps2SqlData.cpp
)

FIND_PROGRAM(UNAME NAMES uname)
EXEC_PROGRAM("${UNAME}" ARGS "-m" OUTPUT_VARIABLE "ARCH")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

# -fPIE and -pie flag is added for ASLR
SET_SOURCE_FILES_PROPERTIES(
    ${DRMTAPPSINITIALIZE_SRCS}
    PROPERTIES
        COMPILE_FLAGS "-fPIE")

ADD_EXECUTABLE(${DRMTAPPSINITIALIZE} ${DRMTAPPSINITIALIZE_SRCS})

TARGET_LINK_LIBRARIES(${DRMTAPPSINITIALIZE}
    ${DRM_INITIALIZE_DEP_LIBRARIES}
    -pie)

#################################################################

CONFIGURE_FILE(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)

INSTALL(FILES ${PROJECT_NAME}.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

INSTALL(FILES resource/ro_root_cert.pem
    DESTINATION ${TZ_SYS_SHARE}/tizen_app_drm/root_certs)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR})
INSTALL(TARGETS ${DRMTAPPSINITIALIZE} DESTINATION bin)

INSTALL(DIRECTORY include/ DESTINATION ${INCLUDEDIR}/drm-tizen/)

#################################################################

IF(DEFINED BUILD_TEST_DRM_SERVICE_CORE_TIZEN)
ADD_SUBDIRECTORY(test)
ENDIF(DEFINED BUILD_TEST_DRM_SERVICE_CORE_TIZEN)
