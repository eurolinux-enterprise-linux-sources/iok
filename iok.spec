Name:           iok
Version:        1.3.13
Release:        3%{?dist}
Summary:        Indic Onscreen Virtual Keyboard
Group:          Applications/System
License:        GPLv2+
URL:            http://iok.sourceforge.net
Source0:        https://fedorahosted.org/releases/i/o/iok/%{name}-%{version}.tar.gz
Patch0:         iok-1.3.13-configure.patch
Patch1:         iok-1.3.13-increase-size-xkb-name-array.patch
Patch2:         iok-1.3.13-save-file-enhancements.patch
Patch3:         iok-1.3.13_translation_bz_819795.patch
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  desktop-file-utils libXtst-devel
BuildRequires:  gtk2-devel gettext libxml2-devel
BuildRequires:  intltool unique-devel
Requires:  xkeyboard-config 

%description
iok is Indic Onscreen Keyboard. It provides virtual Keyboard functionality. 
It currently works with Inscript and xkb keymaps for Indian languages. iok
can even try to parse non-inscript keymaps and show them in iok.

%prep
%setup -q
%patch0 -p1
%patch1 -p0
%patch2 -p1
%patch3 -p1

%build
%configure
make CFLAGS="%{optflags}" %{?_smp_mflags}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot} INSTALL="install -p"

desktop-file-install --vendor fedora \
    --delete-original \
    --copy-generic-name-to-name \
    --dir %{buildroot}/%{_datadir}/applications/ \
     %{buildroot}%{_datadir}/applications/%{name}.desktop

%find_lang %{name}


%clean
rm -rf %{buildroot}

%files -f %{name}.lang
%defattr(-,root,root,-)
%doc COPYING AUTHORS ChangeLog README
%{_bindir}/iok
%{_datadir}/applications/fedora-iok.desktop
%{_datadir}/pixmaps/iok.xpm
%{_mandir}/man1/iok.1.gz


%changelog
* Tue Jul 24 2012 Parag Nemade <pnemade AT redhat DOT com>- 1.3.13-3
- Resolves:rh#814548: Save File option should suggests a predefined file name format
- Resolves:rh#814521: iok does not fetch any .mim file name which does not contain "inscript2" word.
- Resolves:rh#814541: Predefined naming convention followed when save File name, but no clue of that to user 
- Resolves:rh#819795: Translation tracker for iok  

* Tue Mar 06 2012 Parag Nemade <pnemade AT redhat DOT com>- 1.3.13-2
- Resolves:rh#798592 - iok crashed while selecting 'xkb-Malayalam (enhanced Inscript with Rupee Sign)'

* Tue Jan 24 2012 Parag Nemade <pnemade AT redhat DOT com>- 1.3.13-1
- Resolves:rh#752667 - iok looks for ~/.m17n not ~/.m17n.d
- Resolves:rh#752668 - input fail on touch-screen with finger, while mouse click is working for iok
- Resolves:rh#736992 - iok crashed and segfaults occurred while selecting xkb-Hindi Wx keyboard
- Update to 1.3.13 release

* Wed Dec 22 2010 Parag Nemade <pnemade AT redhat.com>- 1.3.10-3
- Resolves:rh#636756:[or_IN][VM] iok is out of VM's Screen

* Thu Jun 03 2010 Parag Nemade <pnemade AT redhat.com>- 1.3.10-2
- Resolves:rh#589213:[iok][ALL_LANG] Translation updates

* Thu Mar 11 2010 Parag Nemade <pnemade AT redhat.com>- 1.3.10-1
- Resolves:rh#572418:Rebase iok to latest version 1.3.10 to add man page and latest translations

* Tue Feb 09 2010 Parag Nemade <panemade@gmail.com>- 1.3.9-1
- Update to Next release 1.3.9

* Tue Oct 26 2009 Parag Nemade <panemade@gmail.com>- 1.3.8-1
- Update to Next release 1.3.8

* Fri Sep 11 2009 Parag Nemade <panemade@gmail.com>- 1.3.7-1
- Update to Next release 1.3.7

* Fri Jul 24 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.3.6-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild

* Mon Jul 20 2009 Parag Nemade <panemade@gmail.com>- 1.3.6-1
- Update to Next release 1.3.6
- Add BR:intltool

* Thu Jun 25 2009 Parag Nemade <panemade@gmail.com>- 1.3.5-1
- Update to Next release 1.3.5
- Resolves: rh506623:iok segfaults when changing the language

* Tue Apr 14 2009 Parag Nemade <panemade@gmail.com>- 1.3.4-1
- Update to Next release 1.3.4

* Fri Mar 20 2009 Parag Nemade <panemade@gmail.com>- 1.3.3-1
- Update to Next release 1.3.3

* Fri Mar 06 2009 Parag Nemade <panemade@gmail.com>- 1.3.2-2
- Resolves: rh#488937:iok should show map list as well as switch button in English locale 

* Thu Mar 05 2009 Parag Nemade <panemade@gmail.com>- 1.3.2-1
- Update to Next release 1.3.2

* Thu Feb 26 2009 Parag Nemade <panemade@gmail.com>- 1.3.1-1
- Update to Next release 1.3.1

* Thu Feb 19 2009 Parag Nemade <panemade@gmail.com>- 1.2.1-1
- Update to Next release 1.2.1

* Tue Jan 20 2009 Parag Nemade <panemade@gmail.com>- 1.2.0-2
- Resolves: rh#480289

* Mon Jan 19 2009 Parag Nemade <panemade@gmail.com>- 1.2.0-1
- Update to Next release 1.2.0

* Tue Jan 13 2009 Parag Nemade <panemade@gmail.com>- 1.1.0-1
- Update to Next release 1.1.0

* Thu Dec 17 2008 Parag Nemade <panemade@gmail.com>- 1.0.9-1
- Update to Next release 1.0.9

* Tue Sep 02 2008 Parag Nemade <panemade@gmail.com>- 1.0.8-2
- Added Source URL and modified description 

* Tue Sep 02 2008 Parag Nemade <panemade@gmail.com>- 1.0.8-1
- Update to Next release 1.0.8

* Thu Aug 14 2008 Parag Nemade <panemade@gmail.com>- 1.0.7-3.svn9
- fix directory ownership

* Thu Aug 14 2008 Parag Nemade <panemade@gmail.com>- 1.0.7-2.svn9
- Update to svn snapshot revision 9

* Tue Jun 17 2008 Parag Nemade <panemade@gmail.com>- 1.0.7-1
- Update to Next release 1.0.7

* Thu Jun 12 2008 Parag Nemade <panemade@gmail.com>- 1.0.6-2
- Added missing BR:libXtst-devel

* Fri Apr 25 2008 Parag Nemade <panemade@gmail.com>- 1.0.6-1
- Update to Next release 1.0.6

* Tue Apr 17 2008 Parag Nemade <panemade@gmail.com>- 1.0.2-1
- Update to Next release 1.0.2

* Tue Apr 15 2008 Parag Nemade <panemade@gmail.com>- 1.0.0-1
- Initial specfile for Fedora 

