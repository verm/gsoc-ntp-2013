/* cmd_args.cpp = unit test cases for command-line argument processing */
#include "ntpdtest.h"

extern "C" {

#include "ntpd.h"
#include "ntp_stdlib.h"
#include "ntp_config.h"
#include "ntp_cmdargs.h"

/* previously defined in /stnp/config.h */
#undef   PACKAGE
#undef   PACKAGE_NAME
#undef   PACKAGE_STRING
#undef   PACKAGE_TARNAME
#undef   RETSIGTYPE
#include "ntpd-opts.h"

#include <autoopts/options.h>


/* option desc for ntpd opts */
extern tOptions ntpdOptions;

};

class cmdArgsTest : public ntpdtest {
public:
    tOptDesc *optDesc;
protected:

	virtual void SetUp() {
        optDesc = ntpdOptions.pOptDesc;
        return;
	}

};

TEST_F(cmdArgsTest, NetworkOptUnchanged) {
    ipv4_works = 1;
    ipv6_works = 1;
    
    getCmdOpts(0, NULL);
    
	EXPECT_EQ(1, ipv4_works);
	EXPECT_EQ(1, ipv6_works);
}

TEST_F(cmdArgsTest, V4NetworkOptChange) {
    opt_state_mask_t PreStateMask;

    /* Change default setting */
    PreStateMask = optDesc[INDEX_OPT_IPV6].fOptState;
    optDesc[INDEX_OPT_IPV6].fOptState = OPTST_SET;

    /* run and check status */
    getCmdOpts(0, NULL);

	EXPECT_EQ(0, ipv4_works);
	EXPECT_EQ(1, ipv6_works);

    /* put enviroments back to normal */
    optDesc[INDEX_OPT_IPV6].fOptState = PreStateMask;
    ipv4_works = 1;
}

TEST_F(cmdArgsTest, V6NetworkOptChange) {
    opt_state_mask_t PreStateMask;

    /* Change default setting */
    PreStateMask = optDesc[INDEX_OPT_IPV4].fOptState;
    optDesc[INDEX_OPT_IPV4].fOptState = OPTST_SET;

    /* run and check status */
    getCmdOpts(0, NULL);

	EXPECT_EQ(1, ipv4_works);
	EXPECT_EQ(0, ipv6_works);

    /* put enviroments back to normal */
    optDesc[INDEX_OPT_IPV4].fOptState = PreStateMask;
    ipv6_works = 1;
}

/* other cases are for other args, like AUTHREQ or CONFIGFILE, etc... */


