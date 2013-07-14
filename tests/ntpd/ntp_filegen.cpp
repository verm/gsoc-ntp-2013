/* test cases for ntp_filegen.c */
#include "libntptest.h"
     
 /* these following segments are copied from ntp_filegen.c */
 extern "C" {
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_string.h"
#include "ntp_calendar.h"
#include "ntp_filegen.h"
#include "ntp_stdlib.h"
};

class ntpFileGenTest : public ntpdtest {
    char *      dir;
    char *      name;
    FILEGEN *   pgen;
    FILEGEN     filegen;

protected:

	virtual void SetUp() {
        dir = ".";
        name = "FileGenTest";
        pgen = &filegen;
        return;
	}

};

/* Test File Gen Reg function */
TEST_F(ntpFileGenTest, FileGenRegister) {  
    FILEGEN * FileGen;

    /* check status before reg */
    FileGen = filegen_get(name);
	EXPECT_EQ(NULL, FileGen);

    /* check status afterfore reg */
    filegen_register(dir, name, pgen);
    FileGen = filegen_get(name);

    /* check values in the struct */
	EXPECT_NE(NULL, FileGen);
    EXPECT_STREQ(FileGen->dir, dir);
    EXPECT_STREQ(FileGen->fname, name);
}

/* Test File Gen Config function,
   Using the result from the last test, FileGenRegister */
TEST_F(ntpFileGenTest, FileGenConfig) {  
    FILEGEN * FileGen;

    FileGen = filegen_get(name);

    /* check values in the struct */
	if(NULL == FileGen)
        return;

    /* change the FileGen type */
    filegen_config(FileGen, dir, name, FILEGEN_NONE, FileGen->flag);
    
    EXPECT_STREQ(FileGen->type, FILEGEN_NONE);
}

/* Test File Gen Setup function,
   Using the result from the last test, FileGenConfig */
TEST_F(ntpFileGenTest, FileGenSetup) {  
    FILEGEN * FileGen;

    FileGen = filegen_get(name);

    /* check values in the struct */
	if(NULL == FileGen)
        return;

    /* change the FileGen type */
    filegen_setup(FileGen, 0);

    /* Check File have been opened */
    EXPECT_NE(FileGen->fp, NULL);

    /* Clean status */
    fclose(FileGen->fp);
    FileGen->fp = NULL;
}

#ifdef DEBUG
/* Test File Gen Unreg function */
TEST_F(ntpFileGenTest, FileGenUnReg) {  
    FILEGEN * FileGen;

    /* unreg FileGen */
    filegen_unregister(name);

    FileGen = filegen_get(name);

    /* Check FileGen have been Unreged */
    EXPECT_EQ(FileGen, NULL);
}

#endif

