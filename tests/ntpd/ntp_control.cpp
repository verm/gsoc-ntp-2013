/*
 * ntp_control.cpp - respond to mode 6 control messages and send async
 *		   traps.  Provides service to ntpq and others.
 */
#include "libntptest.h"
     
 /* these following segments are copied from ntp_config.c */
 extern "C" {

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

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
	EXPECT_EQ(EVNT_UNSPEC, ctl_sys_last_event);
	EXPECT_EQ(0, ctl_sys_num_events);
	EXPECT_EQ(0, num_ctl_traps);
}

/* Test Control peer status */
TEST_F(ntpControlTest, ConfigPeerStatus) {  
    struct peer p;
    u_short res;
    
    p.status = 0;
    p.flags = FLAG_CONFIG|FLAG_AUTHENTIC;
    p.keyid = 1;
    p.reach = 1;
    p.cast_flags = MDF_TXONLY_MASK;

    res = ctlpeerstatus(&p);

    /* check values of status */
	EXPECT_EQ(CTL_PST_CONFIG|
	          CTL_PST_AUTHENABLE|
	          CTL_PST_AUTHENTIC|
	          CTL_PST_REACH|
	          CTL_PST_BCAST,
	          CTL_PEER_STATVAL(res));
    
}

/* Test system status control */
TEST_F(ntpControlTest, ControlSysStatus) {  
    struct peer *prepeer;
    struct peer p;
    u_short res;

    prepeer = sys_peer;
    sys_peer = &p;    
    sys_peer->sstclktype = CTL_SST_TS_NTP;
    
    res = ctlsysstatus();

    /* check values of system source */
	EXPECT_EQ(CTL_SST_TS_NTP, CTL_SYS_SOURCE(res));

    sys_peer = prepeer;
}

/* Test system status control */
TEST_F(ntpControlTest, ControlSysStatus) {  
    struct ctl_var *pre_sys_var;
    struct ctl_var k[2];
    char   *strtest = "test=123";
    char   *tag = "test", *res;
    u_long  size = 9;
    u_short def= 1;

    pre_sys_var = ext_sys_var;
    ext_sys_var = k;
    k[0].text = "test=abc"
    k[0].flags = 0;
    k[0].next = &(k[1]);
    k[1].flags = EOV;
    k[1].next = NULL;
        
    set_sys_var(strtest, size, def);
    res = get_ext_sys_var(tag);

    /* check values of system source */
	EXPECT_STREQ("123", res);

    ext_sys_var = pre_sys_var;
}


