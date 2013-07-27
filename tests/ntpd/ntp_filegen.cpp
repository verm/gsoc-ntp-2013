/* test cases for ntp_filegen.c */
#include "ntpdtest.h"
     
 /* these following segments are copied from ntp_filegen.c */
 extern "C" {
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
protected:
	virtual void SetUp() {
        return;
	}

};

static int ntpFileGenTestpreTestSuccess;
static FILEGEN   ntpFileGenTestRealGen;

/* Test File Gen Reg function */
TEST_F(ntpFileGenTest, FileGenRegister) {  
    FILEGEN * FileGen;
    char       dir[] = ".";
    char       name[] = "FileGenTest";

    /* check status before reg */
    FileGen = filegen_get(name);
	EXPECT_EQ(NULL, FileGen);

    /* check status afterfore reg */
    filegen_register(dir, name, &ntpFileGenTestRealGen);
    FileGen = filegen_get(name);

    /* check values in the struct */
	EXPECT_NE((FILEGEN *)NULL, FileGen);
    EXPECT_STREQ(FileGen->dir, dir);
    EXPECT_STREQ(FileGen->fname, name);
}

/* Test File Gen Config function,
   Using the result from the last test, FileGenRegister */
TEST_F(ntpFileGenTest, FileGenConfig) {  
    FILEGEN * FileGen;
    char       dir[] = ".";
    char       name[] = "FileGenTest";

    FileGen = filegen_get(name);

    /* check values in the struct */
	if(NULL != FileGen)
    { 
        /* change the FileGen type */
        filegen_config(FileGen, dir, name,
                       FILEGEN_NONE, 
                       FileGen->flag);
        
        EXPECT_EQ(FileGen->type, FILEGEN_NONE);
    }
    else
    {
        ntpFileGenTestpreTestSuccess = 0;
    }

}

/* Test File Gen Setup function,
   Using the result from the last test, FileGenConfig */
TEST_F(ntpFileGenTest, FileGenSetup) {  
    FILEGEN * FileGen;
    char       name[] = "FileGenTest";

    FileGen = filegen_get(name);

    /* check values in the struct */
	if(NULL == FileGen)
        return;

    FileGen->flag = FGEN_FLAG_ENABLED;

    /* change the FileGen type */
    filegen_setup(FileGen, 0);

    /* Check File have been opened */
    EXPECT_NE(FileGen->fp, (FILE *)NULL);

    /* Clean status */
    fclose(FileGen->fp);
    FileGen->fp = NULL;
}

/* Test File Gen Unreg function */
TEST_F(ntpFileGenTest, FileGenUnReg) {  
#ifdef DEBUG
    FILEGEN * FileGen;
    char       name[] = "FileGenTest";

    /* unreg FileGen */
    filegen_unregister(name);

    FileGen = filegen_get(name);

    /* Check FileGen have been Unreged */
    EXPECT_EQ(FileGen, (FILEGEN *)NULL);

#endif
}
