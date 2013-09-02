systemdgeneratordir = $(systemddir)/system-generators
systemdunitdir = $(systemddir)/system

systemd_PROGRAMS = systemdescaper

systemdescaper_SOURCES = \
	$(top_srcdir)/systemd/systemdescaper.c

systemdgenerator_SCRIPTS = \
	$(top_srcdir)/systemd/systemd-zfs-generator

systemdunit_DATA = \
	$(top_srcdir)/systemd/zfs-share-nfs.service

EXTRA_DIST = \
	$(top_srcdir)/systemd/zfs-share-nfs.service.in \
	$(top_srcdir)/systemd/systemd-zfs-generator.in \
	$(top_srcdir)/systemd/README.systemd

$(systemdgenerator_SCRIPTS):
	-$(SED) -e 's,@bindir\@,$(bindir),g' \
		-e 's,@sbindir\@,$(sbindir),g' \
		-e 's,@sysconfdir\@,$(sysconfdir),g' \
		-e 's,@systemddir\@,$(systemddir),g' \
		-e 's,@systemdgeneratordir\@,$(systemdgeneratordir),g' \
		'$@.in' >'$@' && chmod +x '$@'

$(systemdunit_DATA):
	-$(SED) -e 's,@bindir\@,$(bindir),g' \
		-e 's,@sbindir\@,$(sbindir),g' \
		-e 's,@sysconfdir\@,$(sysconfdir),g' \
		-e 's,@systemddir\@,$(systemddir),g' \
		-e 's,@systemdunitdir\@,$(systemdunitdir),g' \
		'$@.in' >'$@'

distclean-local::
	-$(RM) $(systemdgenerator_SCRIPTS) $(systemdunit_DATA)

clean-local::
	-$(RM) $(systemdgenerator_SCRIPTS) $(systemdunit_DATA)
