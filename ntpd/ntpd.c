/*
 * ntpd.c - main program for the fixed point NTP daemon
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ntp_machine.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_stdlib.h"
#include <ntp_random.h>

#include "ntp_config.h"
#include "ntp_syslog.h"
#include "ntp_assert.h"
#include "isc/error.h"
#include "isc/strerror.h"
#include "isc/formatcheck.h"

#ifdef SIM
# include "ntpsim.h"
#endif

#include "ntp_libopts.h"
#include "ntpd-opts.h"

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#include <stdio.h>
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#ifdef HAVE_SYS_SIGNAL_H
# include <sys/signal.h>
#else
# include <signal.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif /* HAVE_SYS_IOCTL_H */
#if defined(HAVE_RTPRIO)
# ifdef HAVE_SYS_LOCK_H
#  include <sys/lock.h>
# endif
# include <sys/rtprio.h>
#else
# ifdef HAVE_PLOCK
#  ifdef HAVE_SYS_LOCK_H
#	include <sys/lock.h>
#  endif
# endif
#endif
#if defined(HAVE_SCHED_SETSCHEDULER)
# ifdef HAVE_SCHED_H
#  include <sched.h>
# else
#  ifdef HAVE_SYS_SCHED_H
#   include <sys/sched.h>
#  endif
# endif
#endif
#if defined(HAVE_SYS_MMAN_H)
# include <sys/mman.h>
#endif

#ifdef HAVE_TERMIOS_H
# include <termios.h>
#endif

#ifdef SYS_DOMAINOS
# include <apollo/base.h>
#endif /* SYS_DOMAINOS */

#include "recvbuff.h"
#include "ntp_cmdargs.h"

#ifdef _AIX
# include <ulimit.h>
#endif /* _AIX */

#ifdef SCO5_CLOCK
# include <sys/ci/ciioctl.h>
#endif

#ifdef HAVE_DROPROOT
# include <ctype.h>
# include <grp.h>
# include <pwd.h>
#ifdef HAVE_LINUX_CAPABILITIES
# include <sys/capability.h>
# include <sys/prctl.h>
#endif
#endif

#ifdef SIM
extern  char const *progname;
void parse_cmdline_opts(int *	pargc, char ***pargv);
#ifdef DEBUG
extern  int     debug;
#endif
extern const char *Version;
int ntpsim(int	argc, char * argv[]);
#else	/* !SIM follows */
int ntpdmain(int argc, char *argv[]);
#endif

#ifdef SIM
int
main(
	int argc,
	char *argv[]
	)
{
	progname = argv[0];
	parse_cmdline_opts(&argc, &argv);
#ifdef DEBUG
	debug = OPT_VALUE_SET_DEBUG_LEVEL;
	DPRINTF(1, ("%s\n", Version));
#endif

	return ntpsim(argc, argv);
}
#else	/* !SIM follows */
#ifdef NO_MAIN_ALLOWED
CALL(ntpd,"ntpd",ntpdmain);
#else	/* !NO_MAIN_ALLOWED follows */
#ifndef SYS_WINNT
int
main(
	int argc,
	char *argv[]
	)
{
	return ntpdmain(argc, argv);
}
#endif /* !SYS_WINNT */
#endif /* !NO_MAIN_ALLOWED */
#endif /* !SIM */
