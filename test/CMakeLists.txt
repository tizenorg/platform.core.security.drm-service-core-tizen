pkg_check_modules(DRM_TEST_DEP
    REQUIRED
    cryptsvc
    db-util
    dlog
    openssl
)

SET(DRMTESTUTIL_SRCS
    drm_testapps.cpp
    drm_testcore.cpp
    drm_testutil.cpp
    ${PROJECT_SOURCE_DIR}/service/drm-tapps.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/DrmFileHandleMgr/DrmFileApi.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/DrmFileHandleMgr/DrmFileHandler.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/DrmFileHandleMgr/DrmFileMgr.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/Svc/DrmTdcSvc.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/TADCCore/TADC_Core.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/TADCCore/TADC_Sub.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/TADCCore/TADC_Util.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/TADCInterface/DUIDGenerator.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/TADCInterface/TADC_IF.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/XMLParser/CPointerArray.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/XMLParser/CXMLAttribute.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/XMLParser/CXMLElement.cpp
    ${PROJECT_SOURCE_DIR}/tadcore/XMLParser/CXMLFile.cpp
    ${PROJECT_SOURCE_DIR}/tappsd/src/db/DTapps2SqlData.cpp
    ${PROJECT_SOURCE_DIR}/tappsd/src/db/DTapps2SqliteDB.cpp
    ${PROJECT_SOURCE_DIR}/tappsd/src/intf/drm_intf_tapps.cpp
    ${PROJECT_SOURCE_DIR}/tappsd/src/rights/DTapps2Rights.cpp
    ${PROJECT_SOURCE_DIR}/tappsd/src/util/DTapps2Base64.cpp
    ${PROJECT_SOURCE_DIR}/tappsd/src/util/DTapps2HMAC.cpp
    ${PROJECT_SOURCE_DIR}/tappsd/src/util/DTapps2Time.cpp
)

INCLUDE_DIRECTORIES(
    SYSTEM
    ${DRM_TEST_DEP_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/tadcore/include
    ${PROJECT_SOURCE_DIR}/tappsd/inc
)

ADD_DEFINITIONS("-DTEST_CODE_ENABLED")

SET_SOURCE_FILES_PROPERTIES(
    ${DRMTESTUTIL_SRCS}
    PROPERTIES
        COMPILE_FLAGS "-fPIE")

ADD_EXECUTABLE(${DRMTESTUTIL} ${DRMTESTUTIL_SRCS})

TARGET_LINK_LIBRARIES(${DRMTESTUTIL}
    ${DRM_TEST_DEP_LIBRARIES}
    -ldevice_info
    -pie
)

INSTALL(TARGETS ${DRMTESTUTIL}
    DESTINATION ${BIN_DIR}
    PERMISSIONS
        OWNER_READ
        OWNER_WRITE
        OWNER_EXECUTE
        GROUP_READ
        GROUP_EXECUTE
        WORLD_READ
        WORLD_EXECUTE
)

INSTALL(FILES
    sample_contents/DCF/38EIfBurLJ.tpk
    sample_contents/DCF/8SPXfqc6iL.tpk
    sample_contents/DCF/basiccoreuiapp_drm.tpk
    sample_contents/DCF/FightGuiIF.tpk
    DESTINATION ${TEST_DATA_DIR}/DCF
)

INSTALL(FILES
    sample_contents/RO/38EIfBurLJ-1.0.2.cert_chain_invalid.ro
    sample_contents/RO/38EIfBurLJ-1.0.2.cert_invalid.ro
    sample_contents/RO/38EIfBurLJ-1.0.2.cert_only_selfsigned.ro
    sample_contents/RO/38EIfBurLJ-1.0.2.ro
    sample_contents/RO/38EIfBurLJ-1.0.2.ro_template
    sample_contents/RO/38EIfBurLJ-1.0.2.signature_invalid.ro
    sample_contents/RO/8SPXfqc6iL-1.0.0.ro
    sample_contents/RO/8SPXfqc6iL-1.0.0.signature_invalid.ro
    sample_contents/RO/FightGuiIF-1.0.0.ro
    sample_contents/RO/FightGuiIF-1.0.0.signature_invalid.ro
    DESTINATION ${TEST_DATA_DIR}/RO
)

INSTALL(FILES
    sample_contents/DecryptedApp/38EIfBurLJ_dec.tpk
    sample_contents/DecryptedApp/8SPXfqc6iL_dec.tpk
    sample_contents/DecryptedApp/basiccoreuiapp_signed.tpk
    sample_contents/DecryptedApp/FightGuiIF_dec.tpk
    DESTINATION ${TEST_DATA_DIR}/DecryptedApp
)

INSTALL(FILES
    sample_contents/KEYS/ro_test_signer.key
    DESTINATION ${TEST_DATA_DIR}/Key
)

INSTALL(FILES
    sample_contents/KEYS/ro_test_root_ca.pem
    DESTINATION ${RO_DATA_DIR}/root_certs
)
