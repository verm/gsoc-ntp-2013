NULL =
BUILT_SOURCES =
CLEANFILES =

check_PROGRAMS = tests

LDADD =					\
	$(top_builddir)/ntpd/version.o  \
	$(top_srcdir)/ntpd/ntp_config.o		\
	$(top_srcdir)/ntpd/ntp_io.o		\
	$(top_srcdir)/ntpd/ntp_scanner.o		\
	$(top_srcdir)/ntpd/ntpd-opts.o		\
	$(top_srcdir)/ntpd/ntp_parser.o  \
	$(top_builddir)/ntpd/libntpd.a	\
	$(top_builddir)/libntp/libntp.a	\
	$(LDADD_LIBNTP)			\
	$(LIBOPTS_LDADD)                \
	$(LIBM)                         \
	$(LSCF)                         \
	$(PTHREAD_LIBS)			\
	$(LDADD_NTP)			\
	$(GTEST_LDFLAGS)		\
	$(GTEST_LIBS)			\
	$(NULL)

AM_CFLAGS   = $(CFLAGS_NTP)
AM_CXXFLAGS = $(GTEST_CXXFLAGS)

AM_CPPFLAGS  = $(NTP_INCS)
AM_CPPFLAGS += -I$(top_srcdir)/sntp
AM_CPPFLAGS += -I$(top_srcdir)/sntp/libopts
AM_CPPFLAGS += -I$(top_srcdir)/ntpd
AM_CPPFLAGS += $(GTEST_CPPFLAGS)
AM_CPPFLAGS += $(CPPFLAGS_NTP)
#add by YN
AM_CPPFLAGS += -fprofile-arcs -ftest-coverage

AM_LDFLAGS = $(LDFLAGS_NTP)
#add by YN
AM_LDFLAGS += -lgcov

tests_SOURCES = $(top_srcdir)/sntp/tests_main.cpp	\
	$(top_srcdir)/ntpd/ntpd.c	\
	ntpdtest.cpp            \
	cmd_args.cpp		\
	ntp_config.cpp		\
	$(NULL)

#	$(top_srcdir)/ntpd/ntp_parser.y				
#	$(top_srcdir)/sntp/libopts/usage.c  \
#	$(top_srcdir)/ntpd/ntp_config.c		\
#	$(top_srcdir)/ntpd/ntp_keyword.h		\
#	$(top_srcdir)/ntpd/ntp_io.c		\
#	$(top_srcdir)/ntpd/ntp_scanner.c		\
#	$(top_srcdir)/ntpd/ntp_scanner.h		\
#	$(top_srcdir)/ntpd/ntpd-opts.c		\
#	$(top_srcdir)/ntpd/ntpd-opts.h		
		
noinst_HEADERS = ntpdtest.h	\
			     $(NULL)

TESTS =

if !NTP_CROSSCOMPILE
TESTS += tests
endif

## check-libntp.mf - automake fragment
## slightly adapted for deeper directory

BUILT_SOURCES	+= check-libntp
CLEANFILES	+= check-libntp

check-libntp: ../../libntp/libntp.a
	@echo stamp > $@

../../libntp/libntp.a:
	cd ../../libntp && $(MAKE) $(AM_MAKEFLAGS) libntp.a

include $(top_srcdir)/depsver.mf
include $(top_srcdir)/includes.mf
