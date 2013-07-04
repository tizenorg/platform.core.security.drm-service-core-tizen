Name:       drm-service-core-sapps
Summary:    The plugin library for installing drm-service-core-sapps.so
Version:    0.2.8
Release:    0
Group:      TO_BE/FILLED_IN
License:    Flora
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake

Provides: libdrm-service-core-sapps.so.0

Requires(post): /sbin/ldconfig  
Requires(postun): /sbin/ldconfig

%description
The plugin library for installing drm-service-core-sapps.so

%package devel
Summary:    The plugin library for drm-service-core-sapps(Development)
Group:      TO_BE/FILLED_IN
Requires:   %{name} = %{version}-%{release}

%description devel
The plugin library for installing drm-service-core-sapps.so and header files.

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

%{_bindir}/drm_sapps_initialize

ln -s -f %{_libdir}/libdrm-service-core-sapps.so.0.2.8 %{_libdir}/libdrm-service-core-sapps.so.0
ln -s -f %{_libdir}/libdrm-service-core-sapps.so.0 %{_libdir}/libdrm-service-core-sapps.so

%postun -p /sbin/ldconfig

%files
%manifest drm-service-core-sapps.manifest
%defattr(-,root,root,-)
%{_libdir}/libdrm-service-core-sapps.so*
%{_bindir}/drm_sapps_initialize

%files devel
%defattr(-,root,root,-)
%{_libdir}/pkgconfig/drm-service-core-sapps.pc
%{_includedir}/drm-sapps/drm-oem-sapps.h

%clean
rm -rf drm_sapps_initialize
rm -rf libdrm-service-core-sapps.so*
