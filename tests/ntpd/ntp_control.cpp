/*
 * ntp_control.cpp - respond to mode 6 control messages and send async
 *		   traps.  Provides service to ntpq and others.
 */
#include "ntpdtest.h"
     
/* these following segments are copied from ntp_config.c */
extern "C" {
#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/stat.h>
#ifdef HAVE_NETINET_IN_H
# include <netinet/in.h>
#endif
#include <arpa/inet.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_refclock.h"
#include "ntp_control.h"
#include "ntp_unixtime.h"
#include "ntp_stdlib.h"
#include "ntp_config.h"
#include "ntp_crypto.h"
#include "ntp_assert.h"
#include "ntp_md5.h"	/* provides OpenSSL digest API */
#include "lib_strbuf.h"
#ifdef KERNEL_PLL
# include "ntp_syscall.h"
#endif
};

class ntpControlTest : public ntpdtest {
protected:

	virtual void SetUp() {
        return;
	}

};

/* Test Clear control Stats */
TEST_F(ntpControlTest, ClearControlStat) {  

    ctl_clr_stats();

    /* check values in the struct */
	EXPECT_EQ(0, numctlreq);
	EXPECT_EQ(0, numctlbadpkts);
	EXPECT_EQ(0, numctlresponses);
	EXPECT_EQ(0, numctlfrags);
	EXPECT_EQ(0, numctlerrors);
	EXPECT_EQ(0, numctlfrags);
}

/* Test Init control */
TEST_F(ntpControlTest, InitControl) {  

    init_control();

    /* check values in the struct */
	EXPECT_EQ(0, ctl_auth_keyid);
	EXPECT_EQ(0, num_ctl_traps);
}

/* Test Control peer status */
TEST_F(ntpControlTest, ConfigPeerStatus) {  
    static struct peer p;
    static struct peer* pp;
    u_short res;
    
    p.status = 0;
    p.flags = FLAG_CONFIG|FLAG_AUTHENTIC;
    p.keyid = 1;
    p.reach = 1;
    p.cast_flags = MDF_TXONLY_MASK;

    pp = &p;
    res = ctlpeerstatus(pp);

    /* check values of status */
	EXPECT_EQ(CTL_PST_CONFIG|
	          CTL_PST_AUTHENABLE|
	          CTL_PST_AUTHENTIC|	          
	          CTL_PST_BCAST,
	          CTL_PEER_STATVAL(res));
    
}

/* Test system status control */
TEST_F(ntpControlTest, ControlSysStatus) {  
    struct peer *prepeer;
    static struct peer p;
    u_short res;

    prepeer = sys_peer;
    sys_peer = &p;    
#ifdef REFCLOCK
    sys_peer->sstclktype = CTL_SST_TS_ATOM; 
#endif

    res = ctlsysstatus();

    /* check values of system source */
#ifdef REFCLOCK
	EXPECT_EQ(CTL_SST_TS_ATOM, CTL_SYS_SOURCE(res & 0xffffffff));
#else
    EXPECT_EQ(CTL_SST_TS_NTP, CTL_SYS_SOURCE(res & 0xffffffff));
#endif

    sys_peer = prepeer;
}

/* Test system var control */
TEST_F(ntpControlTest, ControlSysVar) {  
    struct ctl_var *pk;
    struct ctl_var **ppk;
    struct ctl_var k[2];
    char   strtest[] = "test=123";
    char   tag[] = "test";
    const char *res;
    const char *expres = "123";
    u_long  size = 9;
    u_short def= 1;

    k[0].text = (char *)emalloc(size);
    strcpy(k[0].text, "test=abc");
    k[0].flags = 0;
    k[1].flags = EOV;
    pk = &k[0];
    ppk = &pk;

    set_sys_var(strtest, size, def);
    res = get_ext_sys_var(tag);

    /* check values of system source */
	EXPECT_STREQ(expres, res);
}
