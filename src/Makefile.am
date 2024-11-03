bin_PROGRAMS = main
main_SOURCES = main.c utils.c database.c repo.c conf.c install.c build.c parser.c
main_LDADD = $(SQLITE3_LIBS) $(LIBCURL_LIBS) $(OPENSSL_LIBS) $(ARCHIVE_LIBS) $(LIBXML_LIBS)
main_CPPFLAGS = $(SQLITE3_CFLAGS) $(LIBCURL_CFLAGS) $(OPENSSL_CFLAGS) $(ARCHIVE_CFLAGS) $(LIBXML_CFLAGS)
