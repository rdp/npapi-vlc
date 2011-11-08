ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = \
	1.11 \
	-Wall \
	check-news \
	dist-xz \
	no-dist-gzip

DIST_SUBDIRS= share npapi activex common
SUBDIRS = npapi
if BUILD_ACTIVEX
SUBDIRS += activex
endif

EXTRA_DIST = \
	autogen.sh \
	extras/macosx/Info.plist.in \
	extras/macosx/InstallerInfo.plist.in \
	extras/macosx/InstallerDescription.plist \
	extras/macosx/pbdevelopment.plist

package-macosx-plugin:
# Create Installer
	rm -rf "$(top_builddir)/macosx-plugin-instdata/"; true
	mkdir -p "$(top_builddir)/macosx-plugin-instdata"
	rm -f "$(top_builddir)/src/VLC Plugin.plugin/Contents/MacOS/modules/libmacosx_plugin.dylib"
	cp -R "$(top_builddir)/src/VLC Plugin.plugin" "$(top_builddir)/macosx-plugin-instdata"
	rm -rf "$(top_builddir)/VLC Internet Plug-In.pkg"
	export PACKAGEMAKER=
	if test -x /Developer/Tools/packagemaker; then \
	   export PACKAGEMAKER=/Developer/Tools/packagemaker; \
	fi; \
	if test -x /Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker; then \
	   export PACKAGEMAKER=/Developer/Applications/Utilities/PackageMaker.app/Contents/MacOS/PackageMaker; \
	fi; \
	$${PACKAGEMAKER} -build -ds -p "$(top_builddir)/VLC Internet Plug-In.pkg" \
	-f "$(top_builddir)/macosx-plugin-instdata" \
	-i "$(top_builddir)/extras/macosx/InstallerInfo.plist" \
	-d "$(top_srcdir)/extras/macosx/InstallerDescription.plist"; true
	rm -rf "$(top_builddir)/macosx-plugin-instdata/"

# Create disk image
	rm -f "$(top_builddir)/vlc-plugin-$(VERSION).dmg"; true
	rm -rf "$(top_builddir)/vlc-plugin-$(VERSION)/"; true
	mkdir -p "$(top_builddir)/vlc-plugin-$(VERSION)/"
	mv "$(top_builddir)/VLC Internet Plug-In.pkg" "$(top_builddir)/vlc-plugin-$(VERSION)/"
	hdiutil create -srcfolder "$(top_builddir)/vlc-plugin-$(VERSION)" \
	  "$(top_builddir)/vlc-plugin-$(VERSION).dmg" -format UDBZ \
	  -scrub -imagekey zlib-level=9
	echo "Disk image creation completed:"
	rm -rf "$(top_builddir)/vlc-plugin-$(VERSION)"

package-macosx-xpi:
	mkdir -p "$(top_builddir)/macosx-xpi/plugins"
	cp $(top_builddir)/src/install.rdf \
		"$(top_builddir)/macosx-xpi/"
	cp -R "$(top_builddir)/src/VLC Plugin.plugin" \
		"$(top_builddir)/macosx-xpi/plugins"
	cd "$(top_builddir)/macosx-xpi" && zip -r vlc-$(VERSION).xpi install.rdf plugins
