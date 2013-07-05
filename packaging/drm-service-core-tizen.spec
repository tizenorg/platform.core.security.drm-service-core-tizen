Name:       drm-service-core-tizen
Summary:    The plugin library for installing drm-service-core-tizen.so
Version:    0.2.9
Release:    0
Group:      TO_BE/FILLED_IN
License:    Flora
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake

Provides: libdrm-service-core-tizen.so.0

Requires(post): /sbin/ldconfig  
Requires(postun): /sbin/ldconfig

%description
The plugin library for installing drm-service-core-tizen.so

%package devel
Summary:    The plugin library for drm-service-core-tizen (Development)
Group:      TO_BE/FILLED_IN
Requires:   %{name} = %{version}-%{release}

%description devel
The plugin library for installing drm-service-core-tizen.so and header files.

%prep
%setup -q

%build
%ifarch %{arm}
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DARCH=arm
%else
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DARCH=ix86
%endif
make
#make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

%make_install

%post
/bin/rm -f /etc/ld.so.cache
/sbin/ldconfig

%{_bindir}/drm_tizen_initialize

ln -s -f %{_libdir}/libdrm-service-core-tizen.so.0.2.9 %{_libdir}/libdrm-service-core-tizen.so.0
ln -s -f %{_libdir}/libdrm-service-core-tizen.so.0 %{_libdir}/libdrm-service-core-tizen.so

%postun -p /sbin/ldconfig

%files
%manifest drm-service-core-tizen.manifest
%defattr(-,root,root,-)
%{_libdir}/libdrm-service-core-tizen.so*
%{_bindir}/drm_tizen_initialize

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/drm-service-core-tizen.pc
%{_includedir}/drm-tizen/drm-oem-sapps.h

%clean
rm -rf drm_tizen_initialize
rm -rf libdrm-service-core-tizen.so*
