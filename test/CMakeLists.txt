pkg_check_modules(DRM_TEST_DEP
    REQUIRED
	dlog
	db-util
	openssl
    dukgenerator
    cryptsvc
    libtzplatform-config
)

SET(DRMTESTUTIL_SRCS
	${PROJECT_SOURCE_DIR}/test/drm_testapps.cpp
	${PROJECT_SOURCE_DIR}/test/drm_testutil.cpp
	${PROJECT_SOURCE_DIR}/tadcore/TADCCore/TADC_Core.cpp
	${PROJECT_SOURCE_DIR}/tadcore/TADCCore/TADC_Sub.cpp
	${PROJECT_SOURCE_DIR}/tadcore/TADCCore/TADC_Util.cpp
	${PROJECT_SOURCE_DIR}/tadcore/TADCInterface/TADC_IF.cpp
	${PROJECT_SOURCE_DIR}/tadcore/Svc/DrmTdcSvc.cpp
	${PROJECT_SOURCE_DIR}/tappsd/src/intf/drm_intf_tapps.cpp
	${PROJECT_SOURCE_DIR}/tadcore/XMLParser/CPointerArray.cpp
	${PROJECT_SOURCE_DIR}/tadcore/XMLParser/CXMLAttribute.cpp
	${PROJECT_SOURCE_DIR}/tadcore/XMLParser/CXMLElement.cpp
	${PROJECT_SOURCE_DIR}/tadcore/XMLParser/CXMLFile.cpp
	${PROJECT_SOURCE_DIR}/tadcore/DrmFileHandleMgr/DrmFileApi.cpp
	${PROJECT_SOURCE_DIR}/tadcore/DrmFileHandleMgr/DrmFileHandler.cpp
	${PROJECT_SOURCE_DIR}/tadcore/DrmFileHandleMgr/DrmFileMgr.cpp
	${PROJECT_SOURCE_DIR}/tappsd/src/rights/DTapps2Rights.cpp
	${PROJECT_SOURCE_DIR}/tappsd/src/db/DTapps2SqlData.cpp
	${PROJECT_SOURCE_DIR}/tappsd/src/db/DTapps2SqliteDB.cpp
	${PROJECT_SOURCE_DIR}/tappsd/src/util/DTapps2Base64.cpp
	${PROJECT_SOURCE_DIR}/tappsd/src/util/DTapps2HMAC.cpp
	${PROJECT_SOURCE_DIR}/tappsd/src/util/DTapps2Time.cpp
	${PROJECT_SOURCE_DIR}/service/drm-tapps.cpp
	${PROJECT_SOURCE_DIR}/tadcore/TADCInterface/DUIDGenerator.cpp
)

INCLUDE_DIRECTORIES(
	SYSTEM
	${DRM_TEST_DEP_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}/tadcore/include
	${CMAKE_CURRENT_SOURCE_DIR}/tappsd/inc
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

ADD_DEFINITIONS("-DDPL_LOGS_ENABLED")
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
	DESTINATION ${TZ_SYS_BIN}
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
	${PROJECT_SOURCE_DIR}/test/sample_contents/DCF/38EIfBurLJ.tpk
	${PROJECT_SOURCE_DIR}/test/sample_contents/DCF/8SPXfqc6iL.tpk
	${PROJECT_SOURCE_DIR}/test/sample_contents/DCF/FightGuiIF.tpk
	${PROJECT_SOURCE_DIR}/test/sample_contents/DCF/basiccoreuiapp_drm.tpk
	DESTINATION
	${TZ_SYS_DATA}/drm_test/DCF/
)

INSTALL(FILES
	${PROJECT_SOURCE_DIR}/test/sample_contents/RO/38EIfBurLJ-1.0.2.ro
	${PROJECT_SOURCE_DIR}/test/sample_contents/RO/8SPXfqc6iL-1.0.0.ro
	${PROJECT_SOURCE_DIR}/test/sample_contents/RO/FightGuiIF-1.0.0.ro
	${PROJECT_SOURCE_DIR}/test/sample_contents/RO/38EIfBurLJ-1.0.2.cert_chain_invalid.ro
	${PROJECT_SOURCE_DIR}/test/sample_contents/RO/38EIfBurLJ-1.0.2.cert_invalid.ro
	${PROJECT_SOURCE_DIR}/test/sample_contents/RO/38EIfBurLJ-1.0.2.cert_only_selfsigned.ro
	${PROJECT_SOURCE_DIR}/test/sample_contents/RO/38EIfBurLJ-1.0.2.signature_invalid.ro
	${PROJECT_SOURCE_DIR}/test/sample_contents/RO/8SPXfqc6iL-1.0.0.signature_invalid.ro
	${PROJECT_SOURCE_DIR}/test/sample_contents/RO/FightGuiIF-1.0.0.signature_invalid.ro
	${PROJECT_SOURCE_DIR}/test/sample_contents/RO/38EIfBurLJ-1.0.2.ro_template
	DESTINATION
	${TZ_SYS_DATA}/drm_test/RO/
)

INSTALL(FILES
	${PROJECT_SOURCE_DIR}/test/sample_contents/DecryptedApp/38EIfBurLJ_dec.tpk
	${PROJECT_SOURCE_DIR}/test/sample_contents/DecryptedApp/8SPXfqc6iL_dec.tpk
	${PROJECT_SOURCE_DIR}/test/sample_contents/DecryptedApp/FightGuiIF_dec.tpk
	${PROJECT_SOURCE_DIR}/test/sample_contents/DecryptedApp/basiccoreuiapp_signed.tpk
	DESTINATION
	${TZ_SYS_DATA}/drm_test/DecryptedApp/
)

INSTALL(FILES
	${PROJECT_SOURCE_DIR}/test/sample_contents/KEYS/ro_test_signer.key
	DESTINATION
	${TZ_SYS_DATA}/drm_test/Key/
)

INSTALL(FILES
	${PROJECT_SOURCE_DIR}/test/sample_contents/KEYS/ro_test_root_ca.pem
	DESTINATION
	${TZ_SYS_SHARE}/tizen_app_drm/root_certs/
)

