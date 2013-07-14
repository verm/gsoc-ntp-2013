/* ntp_config.cpp = unit test cases for ntpd config processing */
#include "libntptest.h"

/* these following segments are copied from ntp_config.c */
extern "C" {
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_NETINFO
# include <netinfo/ni.h>
#endif

#include <stdio.h>
#include <ctype.h>
#ifdef HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#include <signal.h>
#ifndef SIGCHLD
# define SIGCHLD SIGCLD
#endif
#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#include <isc/net.h>
#include <isc/result.h>

#include "ntp.h"
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_unixtime.h"
#include "ntp_refclock.h"
#include "ntp_filegen.h"
#include "ntp_stdlib.h"
#include "lib_strbuf.h"
#include "ntp_assert.h"
#include "ntp_random.h"
/*
 * [Bug 467]: Some linux headers collide with CONFIG_PHONE and CONFIG_KEYS
 * so #include these later.
 */
#include "ntp_config.h"
#include "ntp_cmdargs.h"
#include "ntp_scanner.h"
#include "ntp_parser.h"
#include "ntpd-opts.h"
};

class ntpConfigTest : public ntpdtest {
protected:

	virtual void SetUp() {
        return;
	}

};

/* Test append_gen_fifo when passed the NULL pointer,
   The normal case can be tested from another case of ntp_config */
TEST_F(ntpConfigTest, GenFifoWithNewBuf) {  
	void *fifo;
	void *entry;
    void *res;

    fifo = NULL;
    entry = NULL;
    res = append_gen_fifo(fifo, entry);

    /* a new block of memory have been allocated */
	EXPECT_NE(NULL, res);

    free(res);
}

/* Test create attr_ival struct */
TEST_F(ntpConfigTest, CreateAttrIval) {  
	attr_val *res;
    int attr,ival;

    attr = 1;
    ival = 2;
    res = create_attr_ival(attr, ival);

    /* check values in the struct */
	EXPECT_EQ(attr, res->attr);
	EXPECT_EQ(ival, res->ival);
	EXPECT_EQ(T_Integer, res->type);

    free(res);
}

/* Test create attr_uval struct */
TEST_F(ntpConfigTest, CreateAttrUval) {  
	attr_val *res;
    int attr;
    u_int uval;

    attr = 2;
    uval = 7;
    res = create_attr_uval(attr, uval);

    /* check values in the struct */
	EXPECT_EQ(attr, res->attr);
	EXPECT_EQ(uval, res->value.u);
	EXPECT_EQ(T_U_int, res->type);

    free(res);
}

/* Test create attr_rangeval struct */
TEST_F(ntpConfigTest, CreateAttrRangeval) {  
	attr_val *res;
    int attr, first, last;

    attr = 3;
    first = -1;
    last = 7;
    res = create_attr_rangeval(attr, first, last);

    /* check values in the struct */
	EXPECT_EQ(attr, res->attr);
	EXPECT_EQ(first, res->value.r.first);
	EXPECT_EQ(last, res->value.r.last);
	EXPECT_EQ(T_Intrange, res->type);

    free(res);
}

/* Test create attr_sval struct with NULL pointer */
TEST_F(ntpConfigTest, CreateNullAttrSval) {  
	attr_val *res;

    attr = 4;
    res = create_attr_sval(attr, NULL);

    /* check values in the struct */
	EXPECT_EQ(attr, res->attr);
	EXPECT_NE(NULL, res->value.s);
	EXPECT_EQ(T_String, res->type);

    free(res);
}

/* Test create attr_sval struct with NULL pointer */
TEST_F(ntpConfigTest, CreateRealAttrSval) {  
	attr_val *res;
    char *strtest = "test";

    attr = 4;
    res = create_attr_sval(attr, strtest);

    /* check values in the struct */
	EXPECT_EQ(attr, res->attr);
	EXPECT_EQ(strtest, res->value.s);
	EXPECT_EQ(T_String, res->type);

    free(res);
}

/* Test create int node */
TEST_F(ntpConfigTest, CreateIntNode) {  
	int_node *res;
    int val;

    val = 2;
    res = create_int_node(val);

    /* check values in the struct */
	EXPECT_EQ(val, res->i);

    free(res);
}

/* Test create string node */
TEST_F(ntpConfigTest, CreateStringNode) {  
	string_node *res;
    char *strtest = "test";

    res = create_string_node(strtest);

    /* check values in the struct */
	EXPECT_EQ(strtest, res->s);

    free(res);
}

/* Test create address node */
TEST_F(ntpConfigTest, CreateAddrNode) {  
	address_node *res;
    int type = AF_INET;
    char *addr = "192.168.1.1";

    res = create_address_node(addr, type);

    /* check values in the struct */
	EXPECT_EQ(addr, res->address);
	EXPECT_EQ(type, res->type);

    free(res);
}

/* Test create address node */
TEST_F(ntpConfigTest, CreateAddrNode) {  
	address_node *res;
    int type = AF_INET;
    char *addr = "192.168.1.1";

    res = create_address_node(addr, type);

    /* check values in the struct */
	EXPECT_EQ(addr, res->address);
	EXPECT_EQ(type, res->type);

    free(res);
}

/* Test create peer node */
TEST_F(ntpConfigTest, CreatePeerNode) {  
	peer_node *res;
    address_node *addrnode;
    int type = AF_INET;
    int hmode = 1;
    char *addr = "192.168.1.1";
    attr_val_fifo fifo[1];

    addrnode = create_address_node(addr, type);
    fifo[0].attr = T_Flag;    

    res = create_peer_node(hmode, addrnode, fifo);

    /* check values in the struct */
	EXPECT_EQ(addr, res->addr);
	EXPECT_EQ(hmode, res->host_mode);

    free(res);
}

