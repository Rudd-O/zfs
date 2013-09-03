pkgdracut99dir = $(dracutdir)/modules.d/99zfssystemd
pkgdracut99_SCRIPTS = \
	$(top_srcdir)/dracut/99zfssystemd/module-setup.sh

EXTRA_DIST = \
	$(top_srcdir)/dracut/99zfssystemd/module-setup.sh.in

$(pkgdracut99_SCRIPTS):
	-$(SED) -e 's,@bindir\@,$(bindir),g' \
		-e 's,@sbindir\@,$(sbindir),g' \
		-e 's,@datadir\@,$(datadir),g' \
		-e 's,@dracutdir\@,$(dracutdir),g' \
		-e 's,@udevdir\@,$(udevdir),g' \
		-e 's,@udevruledir\@,$(udevruledir),g' \
		-e 's,@sysconfdir\@,$(sysconfdir),g' \
		'$@.in' >'$@'

clean-local::
	-$(RM) $(pkgdracut99_SCRIPTS)

distclean-local::
	-$(RM) $(pkgdracut99_SCRIPTS)
