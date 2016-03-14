CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(drm-service-core-tizen CXX)

SET(DRMTESTUTIL drm_test_tizen)
SET(DRMTAPPSINITIALIZE drm_tizen_initialize)

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    ADD_DEFINITIONS("-DDLOG_DEBUG_ENABLE")
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

INCLUDE(FindPkgConfig)

ADD_DEFINITIONS("-DISSUER_ROOT_CERTS_DIR=\"${RO_DATA_DIR}/root_certs\"")
ADD_DEFINITIONS("-DDB_PATH=\"${DB_DIR}/.dtapps.db\"")

############################# compiler flags ##################################
SET(CXX_SPECIFIC_FLAGS         "-std=c++0x")
SET(CMAKE_C_FLAGS_PROFILING    "-g -O0 -Wp,-U_FORTIFY_SOURCE -pg")
SET(CMAKE_CXX_FLAGS_PROFILING  "-g -O0 -Wp,-U_FORTIFY_SOURCE -pg ${CXX_SPECIFIC_FLAGS}")
SET(CMAKE_C_FLAGS_DEBUG        "-g -O0 -Wp,-U_FORTIFY_SOURCE -ggdb")
SET(CMAKE_CXX_FLAGS_DEBUG      "-g -O0 -Wp,-U_FORTIFY_SOURCE -ggdb ${CXX_SPECIFIG_CLAGS}")
SET(CMAKE_C_FLAGS_RELEASE      "-g -O2")
SET(CMAKE_CXX_FLAGS_RELEASE    "-g -O2 ${CXX_SPECIFIC_FLAGS}")
SET(CMAKE_C_FLAGS_CCOV         "${CMAKE_C_FLAGS_RELEASE} --coverage")
SET(CMAKE_CXX_FLAGS_CCOV       "${CMAKE_CXX_FLAGS_RELEASE} --coverage")

ADD_DEFINITIONS("-fPIC")

# Set compiler warning flags
ADD_DEFINITIONS("-Werror")                      # Make all warnings into errors.
ADD_DEFINITIONS("-Wall")                        # Generate all warnings
ADD_DEFINITIONS("-Wextra")                      # Generate even more extra warnings

PKG_CHECK_MODULES(DRM_SERVICE_CORE_DEP
    REQUIRED
    cryptsvc
    db-util
    dlog
    openssl
)

INCLUDE_DIRECTORIES(
    SYSTEM
    ${DRM_SERVICE_CORE_DEP_INCLUDE_DIRS}
    include
    tadcore/include
    tappsd/inc
)

SET(SRCS
    service/drm-tapps.cpp
    tadcore/DrmFileHandleMgr/DrmFileApi.cpp
    tadcore/DrmFileHandleMgr/DrmFileHandler.cpp
    tadcore/DrmFileHandleMgr/DrmFileMgr.cpp
    tadcore/Svc/DrmTdcSvc.cpp
    tadcore/TADCCore/TADC_Core.cpp
    tadcore/TADCCore/TADC_Sub.cpp
    tadcore/TADCCore/TADC_Util.cpp
    tadcore/TADCInterface/DUIDGenerator.cpp
    tadcore/TADCInterface/TADC_IF.cpp
    tadcore/XMLParser/CPointerArray.cpp
    tadcore/XMLParser/CXMLAttribute.cpp
    tadcore/XMLParser/CXMLElement.cpp
    tadcore/XMLParser/CXMLFile.cpp
    tappsd/src/db/DTapps2SqlData.cpp
    tappsd/src/db/DTapps2SqliteDB.cpp
    tappsd/src/intf/drm_intf_tapps.cpp
    tappsd/src/rights/DTapps2Rights.cpp
    tappsd/src/util/DTapps2Base64.cpp
    tappsd/src/util/DTapps2HMAC.cpp
    tappsd/src/util/DTapps2Time.cpp
)

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})

SET_TARGET_PROPERTIES(${PROJECT_NAME}
    PROPERTIES
        COMPILE_FLAGS " -fvisibility=hidden"
        SOVERSION ${MAJORVER}
        VERSION ${FULLVER}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    ${DRM_SERVICE_CORE_DEP_LIBRARIES}
    -ldevice_info # libdevice_info.so in cryptsvc.
)

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR})

#################################################################
PKG_CHECK_MODULES(DRM_INITIALIZE_DEP
    REQUIRED
    db-util
    dlog
    openssl
)

INCLUDE_DIRECTORIES(${DRM_INITIALIZE_DEP_INCLUDE_DIRS})

SET(DRMTAPPSINITIALIZE_SRCS
    service/drm-tapps-initialize.cpp
    tappsd/src/db/DTapps2SqlData.cpp
    tappsd/src/db/DTapps2SqliteDB.cpp
    tappsd/src/intf/drm_intf_tapps.cpp
)

SET_SOURCE_FILES_PROPERTIES(${DRMTAPPSINITIALIZE_SRCS}
    PROPERTIES
        COMPILE_FLAGS "-fPIE"
)

ADD_EXECUTABLE(${DRMTAPPSINITIALIZE} ${DRMTAPPSINITIALIZE_SRCS})

TARGET_LINK_LIBRARIES(${DRMTAPPSINITIALIZE}
    ${DRM_INITIALIZE_DEP_LIBRARIES}
    -pie
)

INSTALL(TARGETS ${DRMTAPPSINITIALIZE} DESTINATION ${BIN_DIR})
#################################################################

CONFIGURE_FILE(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
INSTALL(FILES ${PROJECT_NAME}.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

INSTALL(FILES resource/ro_root_cert.pem DESTINATION ${RO_DATA_DIR}/root_certs)

INSTALL(FILES include/drm-tizen-apps.h include/drm-tizen-error.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/drm-tizen
)

#################################################################

IF(DEFINED BUILD_TEST_DRM_SERVICE_CORE_TIZEN)
    ADD_DEFINITIONS("-DTEST_DATA_DIR=\"${TEST_DATA_DIR}\"")
    ADD_SUBDIRECTORY(test)
ENDIF(DEFINED BUILD_TEST_DRM_SERVICE_CORE_TIZEN)
