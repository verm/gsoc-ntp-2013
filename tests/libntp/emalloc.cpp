/* This file contains test for libntp/emalloc.c
   There are 2 main apis for emalloc.c, one is emalloc, 
   the other is estrdup_impl. emalloc() can be tested by other ut cases. */

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


class emallocTest : public libntptest {
protected:

	virtual void SetUp() {
        return;
	}

};

/* Note, if defined EREALLOC_CALLSITE, that means we are using debug mode.
   In fact, there is no need to test string dump in debug mode. */
TEST_F(emallocTest, TestStringDump) {
    const char *teststr = "TestStringDump";
    char *newstring;
	size_t	bytes;
    int i,res;
#ifdef EREALLOC_CALLSITE
	const char *	file = "./strdump";
	int		line =1;
#endif

	bytes = strlen(teststr) + 1;

    newstring = estrdup_impl(teststr
#ifdef EREALLOC_CALLSITE
        ,file, line
#endif
        );

    /* if newstring == NULL, program will exit automatically,
       so don't need to worry about that case */
    res=0;
    for (i=0; i<bytes; ++i)
    {
        if (newstring[i] != teststr[i])
        {
            res = 1;
            break;
        }
    }
    
    /* confirm that every byte has been copied correctly */
	EXPECT_EQ(res, 0);
}

