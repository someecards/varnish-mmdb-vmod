%if ! 0%{?dist:1}
%define dist .el6
%endif
%define checkoutName %{name}-%{version}-%{_build_arch}-%{suffix: %{dist}}
%define source https://github.com/maxmind/libmaxminddb.git

Name:		libmaxminddb
Version:	1.0
Release:	1%{?dist}
Summary:	libmaxminddb library

Group:		MaxMind
License:	Proprietary
URL:		https://www.maxmind.com/

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-%{_build_arch}-root-%(%{__id_u} -n)
BuildRequires:	git


%description
This provides the MaxMind GeoIP library funtions.

%prep
#prep
rm -rf "%{checkoutName}"
src="%{source}"
if [[ "${src:0:7}" = "file://" ]] ; then :
    cp -R "${src:7}" "%{checkoutName}"
else
    git clone --recursive "%{source}" "%{checkoutName}"
    cd "%{checkoutName}"
fi



%build
#build
cd %{checkoutName}
mkdir buildinfo
echo "%{source}" >> buildinfo/source.txt
git branch > buildinfo/branch.txt
git status > buildinfo/status.txt
git remote --verbose show -n origin > buildinfo/origin.txt
git log --max-count=1 --pretty=fuller > buildinfo/log.txt
./bootstrap
#cd t
#rm -rf libtap
#git clone git://github.com/zorgnax/libtap.git libtap
#cd ..
%configure
make %{?_smp_mflags}


%install
#install
cd %{checkoutName}
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
#clean
rm -rf $RPM_BUILD_ROOT
rm -rf %{buildroot}
rm -rf %{checkoutName}

%files
%defattr(-,root,root,-)
   /usr/*
%doc



%changelog
