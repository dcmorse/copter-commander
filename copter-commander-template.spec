Summary: A 2d network helicopter game.
Name: copter-commander
Version: COCO_MAJOR_VERSION.COCO_MINOR_VERSION
Release: COCO_RPM_RELEASE
Copyright: GPL
Group: Amusements/Games
Source: http://ftp1.sourceforge.net/coco/copter-commander-COCO_MAJOR_VERSION.COCO_MINOR_VERSION.tar.gz
#BuildRoot: /usr/src/redhat/copter-commander

%description
Copter Commander is a side scrolling helicopter game, about 70% arcade
action, 30% realtime strategy.  Play against other people over the internet.

%prep
%setup

%build
#Don't include $RPM_BUILD_ROOT here
make COCO_INSTALL_DIRECTORY=/usr COCO_OPTIMIZATION_FLAGS="$RPM_OPT_FLAGS" 

%install
#include $RPM_BUILD_ROOT here, it'll be stripped off when the package is
#actually installed.
make COCO_INSTALL_DIRECTORY=$RPM_BUILD_ROOT/usr install ;

%clean
if [ "$RPM_BUILD_ROOT" != "/" ]
  then
    if [ -n "$RPM_BUILD_ROOT" ]
      then rm -rf $RPM_BUILD_ROOT ;
    fi ;
  else echo Cowardly refusing to rm -rf / ;
fi ;


%files
/usr/bin/copter-commander
/usr/bin/glx-copter-commander
/usr/share/copter-commander/COCO_MAJOR_VERSION.COCO_MINOR_VERSION

%changelog
* Sat Mar 17 2001 David Morse <copter-commander-devel@bomberlan.net>
- created


