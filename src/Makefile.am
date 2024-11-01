bin_PROGRAMS = main
main_SOURCES = main.c utils.c database.c repo.c conf.c
main_LDADD = $(SQLITE3_LIBS) $(LIBCURL_LIBS)
main_CPPFLAGS = $(SQLITE3_CFLAGS) $(LIBCURL_CFLAGS)