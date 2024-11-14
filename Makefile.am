SUBDIRS = src
dist_doc_DATA = README.md

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = src/*.h examples share

install-exec-hook:
	$(MKDIR_P) $(DESTDIR)$(datarootdir)/eggpm
	for file in share/eggpm/*; do \
		$(INSTALL) -m 755 $$file $(DESTDIR)$(datarootdir)/eggpm/; \
	done