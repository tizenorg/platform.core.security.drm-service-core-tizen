%define test_build_drm_service_core_tizen 0

Name:       drm-service-core-tizen
Summary:    Tizen Application DRM library (Shared Object)
Version:	0.3.9
Release:    0
Group:      System/Security
License:    Flora-1.1 and Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(openssl)
BuildRequires:  pkgconfig(cryptsvc)
BuildRequires:  pkgconfig(dukgenerator)
BuildRequires:  pkgconfig(libtzplatform-config)
Requires: pkgconfig(libtzplatform-config)

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
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake .  -DFULLVER=%{version} \
         -DMAJORVER=${MAJORVER} \
%if 0%{?test_build_drm_service_core_tizen}
         -DBUILD_TEST_DRM_SERVICE_CORE_TIZEN=1 \
%endif
         -DPREFIX=%{_prefix} \
         -DEXEC_PREFIX=%{_exec_prefix} \
         -DLIBDIR=%{_libdir} \
         -DBINDIR=%{_bindir} \
         -DINCLUDEDIR=%{_includedir} \
         -DTZ_SYS_SHARE=%TZ_SYS_SHARE \
         -DTZ_SYS_BIN=%TZ_SYS_BIN \
         -DTZ_SYS_DATA=%TZ_SYS_DATA \
         -DTZ_SYS_ETC=%TZ_SYS_ETC \
         -DTZ_SYS_RO_WRT_ENGINE=%TZ_SYS_RO_WRT_ENGINE \
         -DTZ_SYS_DB=%TZ_SYS_DB \
         -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}%{!?build_type:RELEASE} \
         -DCMAKE_INSTALL_PREFIX=%{_prefix}

make

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{TZ_SYS_SHARE}/license/%{name}
cp  %{_builddir}/%{name}-%{version}/LICENSE.* %{buildroot}%{TZ_SYS_SHARE}/license/%{name}
%make_install

%post
/bin/rm -f /etc/ld.so.cache
/sbin/ldconfig
%{_bindir}/drm_tizen_initialize
chsmack -a "org.tizen.tsefl" %{TZ_SYS_DB}/.dtapps.db*
chown :5000 %{TZ_SYS_DB}/.dtapps.db*
chmod 660 %{TZ_SYS_DB}/.dtapps.db*

%postun -p /sbin/ldconfig

%clean
rm -rf %{buildroot}

%files
%manifest drm-service-core-tizen.manifest
%defattr(-,root,root,-)
%{_libdir}/libdrm-service-core-tizen.so*
%{_bindir}/drm_tizen_initialize
%{TZ_SYS_SHARE}/license/%{name}/*
%{TZ_SYS_SHARE}/tizen_app_drm/root_certs/ro_root_cert.pem

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/drm-service-core-tizen.pc
%{_includedir}/drm-tizen/drm-tizen-apps.h
%{_includedir}/drm-tizen/drm-tizen-error.h
%exclude %{_includedir}/drm-tizen/drm-tizen-mid.h

%if 0%{?test_build_drm_service_core_tizen}
%files test
#%defattr(-,root,root,-)
%{_bindir}/drm_test_tizen
%{TZ_SYS_DATA}/drm_test/RO/*
%{TZ_SYS_DATA}/drm_test/DCF/*
%{TZ_SYS_DATA}/drm_test/DecryptedApp/*
%{TZ_SYS_DATA}/drm_test/Key/*
%{TZ_SYS_SHARE}/tizen_app_drm/root_certs/ro_test_root_ca.pem
%endif
