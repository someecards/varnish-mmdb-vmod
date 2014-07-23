%define checkoutName %{name}-%{version}-%{_build_arch}-%{suffix: %{dist}}
%define source https://github.com/varnish/Varnish-Cache.git

Name:		varnish-cache
Version:	3.0
Release:	5%{?dist}
Summary:	A varnish module to do IP lookup using libmaxminddb
Group:		Varnish.org
License:	Proprietary
URL:		https://github.com/varnish/Varnish-Cache
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-%{_build_arch}-root-%(%{__id_u} -n)
BuildRequires:	git


%description
This provides varnish cache.

%prep
#prep
rm -rf "%{checkoutName}"
git clone "%{source}" "%{checkoutName}"
cd "%{_builddir}/%{checkoutName}"
git branch 3.0 -t origin/3.0
git checkout 3.0
git checkout 1a89b1f75895bbf874e83cfc6f6123737a3fd76f

%build
#build
cd %{checkoutName}
mkdir buildinfo
echo "%{source}" >> buildinfo/source.txt
git branch > buildinfo/branch.txt
git status > buildinfo/status.txt
git remote --verbose show -n origin > buildinfo/origin.txt
git log --max-count=1 --pretty=fuller > buildinfo/log.txt
./autogen.sh
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
/etc/*
%doc



%changelog
