%define test_build_drm_service_core_tizen 0

Name:       drm-service-core-tizen
Summary:    Tizen Application DRM library (Shared Object)
Version:    0.3.9
Release:    0
Group:      System/Security
License:    Flora-1.1 and Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(openssl)
BuildRequires:  pkgconfig(cryptsvc)
BuildRequires:  pkgconfig(libtzplatform-config)

%global bin_dir %{?TZ_SYS_BIN:%TZ_SYS_BIN}%{!?TZ_SYS_BIN:%_bindir}
%global ro_data_dir %{?TZ_SYS_RO_SHARE:%TZ_SYS_RO_SHARE/tizen_app_drm}%{!?TZ_SYS_RO_SHARE:%_datadir/tizen_app_drm}
%global test_data_dir %{?TZ_SYS_RO_SHARE:%TZ_SYS_RO_SHARE/drm_test}%{!?TZ_SYS_RO_SHARE:%_datadir/drm_test}
%global db_dir %{?TZ_SYS_DB:%TZ_SYS_DB}%{!?TZ_SYS_DB:/opt/dbspace}

%description
Description: Tizen Application DRM library (Shared Object)

%package devel
Summary:    Tizen Application DRM library (Shared Object)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Description: Tizen Application DRM library (Shared Object)

%if 0%{?test_build_drm_service_core_tizen}
%package test
Summary:    Tizen Application DRM library (test)
Group:      System/Misc
Requires:   %{name} = %{version}-%{release}

%description test
Description: Tizen Applicationi DRM library (test)
%endif

%prep
%setup -q

%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
export LDFLAGS="$LDFLAGS -Wl,--rpath=%{_litdir}"

MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake .  -DFULLVER=%{version} \
        -DMAJORVER=${MAJORVER} \
%if 0%{?test_build_drm_service_core_tizen}
        -DBUILD_TEST_DRM_SERVICE_CORE_TIZEN=1 \
        -DTEST_DATA_DIR=%{test_data_dir} \
%endif
        -DBIN_DIR=%{bin_dir} \
        -DRO_DATA_DIR=%{ro_data_dir} \
        -DDB_DIR=%{db_dir} \
        -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}%{!?build_type:RELEASE}

make %{?_smp_mflags}

%install
%make_install

%post
%{bin_dir}/rm -f /etc/ld.so.cache
/sbin/ldconfig
%{bin_dir}/drm_tizen_initialize
chsmack -a "System" %{db_dir}/.dtapps.db*
chown :users %{db_dir}/.dtapps.db*
chmod 660 %{db_dir}/.dtapps.db*

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%license LICENSE.*
%{_libdir}/lib%{name}.so.*
%{bin_dir}/drm_tizen_initialize
%{ro_data_dir}/root_certs/ro_root_cert.pem

%files devel
%{_libdir}/pkgconfig/%{name}.pc
%{_libdir}/lib%{name}.so
%{_includedir}/drm-tizen/drm-tizen-apps.h
%{_includedir}/drm-tizen/drm-tizen-error.h

%if 0%{?test_build_drm_service_core_tizen}
%files test
%{bin_dir}/drm_test_tizen
%{test_data_dir}/RO
%{test_data_dir}/DCF
%{test_data_dir}/DecryptedApp
%{test_data_dir}/Key
%{ro_data_dir}/root_certs/ro_test_root_ca.pem
%endif
