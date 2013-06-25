/* This file contains test for both libntp/authkeys.c and libntp/authusekey.c */

#include "libntptest.h"

extern "C" {
#ifdef OPENSSL
# include "openssl/err.h"
# include "openssl/rand.h"
# include "openssl/evp.h"
#endif
#include "ntp.h"
#include "ntp_stdlib.h"
};

static int authReadHaveConfig;

class authReadkeysTest : public libntptest {
protected:
	static const int KEYTYPE = KEY_TYPE_MD5;    

	virtual void SetUp() {

        const char *pConfig = "1 M test";
        FILE    *fp; 
        int writebytes, configlen;

		/*
		 * init_auth() is called by tests_main.cpp earlier.  It
		 * does not initialize global variables like
		 * authnumkeys, so let's reset them to zero here.
		 */
		authnumkeys = 0;

		/*
		 * Especially, empty the key cache!
		 */
		cache_keyid = 0;
		cache_type = 0;
		cache_flags = 0;
		cache_secret = NULL;
		cache_secretsize = 0;

        /* Write an easy useable config file */
        authReadHaveConfig = 0;
        fp = fopen("KeyConfigFile", "w");
        writebytes = 0;
        if (NULL != fp)
        {
            writebytes = fputs(pConfig, fp);
            fclose(fp);
        }
        configlen = strlen(pConfig);
        if (configlen == writebytes)
        {
            /* If successfully wrote the config file, 
               then we will test the real read key function*/
            authReadHaveConfig = 1;
        }
        
	}

};

/* Add by Yan */
TEST_F(authReadkeysTest, KeyFileNotExist) {
    const char *file = "./KeyFileNotExist";
	int res;

    res = authreadkeys(file);

    /* confirm the return value is 0 and no auth keys exist*/
	EXPECT_EQ(res, 0);
    EXPECT_EQ(authnumkeys, 0);
}

/* Add by Yan */
TEST_F(authReadkeysTest, LoadOneConfigSuccess) {
    const char *pConfig = "KeyConfigFile";
	int res;

    if (authReadHaveConfig)
    {
        res = authreadkeys(pConfig);
        
        /* confirm the return value is 1 and an auth key exist*/
        EXPECT_EQ(res, 1);
        EXPECT_EQ(authnumkeys, 1);
    
        /* clean out keys, set environment for next UT case */
        auth_delkeys();
    }
}

