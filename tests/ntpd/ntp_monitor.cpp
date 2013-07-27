/* ntp_monitor - monitor ntpd statistics */
#include "ntpdtest.h"
     
 /* these following segments are copied from ntp_monitor.c */
 extern "C" {
#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_if.h"
#include "ntp_lists.h"
#include "ntp_stdlib.h"
#include <ntp_random.h>

#include <stdio.h>
#include <signal.h>
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
};

class ntpMonitorTest : public ntpdtest {
protected:
	virtual void SetUp() {
	}

};

/* Confirm init_mon will set mon_enable to OFF */
TEST_F(ntpMonitorTest, InitMon) {  
    u_int preStatus;

    preStatus = mon_enabled;
    mon_enabled = MON_RES;

    init_mon();

    EXPECT_EQ(MON_OFF, mon_enabled);

    mon_enabled = preStatus;
}

/* Start with the right mode,
   Make sure it is the first time to run mon_start */
TEST_F(ntpMonitorTest, StartMonFristTime) {  
	u_int pre_mru_alloc;
    
    mon_enabled = MON_OFF;
    pre_mru_alloc = mru_alloc;

    mon_start(MON_ON);

    EXPECT_EQ(MON_ON, mon_enabled);
    EXPECT_EQ(mru_initalloc, mru_alloc - pre_mru_alloc);
}

/* Start an already started monitor,
   Rely on previous test case */
TEST_F(ntpMonitorTest, StartMonSecondTime) {  

    mon_start(MON_RES);

    EXPECT_EQ(MON_RES|MON_ON, mon_enabled);
}

/* Start with the wrong mode,
   Rely on previous test case */
TEST_F(ntpMonitorTest, StartMonWithWrongInput) {  
    mon_start(MON_OFF);

    EXPECT_NE(MON_OFF, mon_enabled);
}

/* Stop an already started monitor,
   Rely on previous test case */
TEST_F(ntpMonitorTest, StopMon) {  

    mon_stop(MON_RES|MON_ON);

    EXPECT_EQ(MON_OFF, mon_enabled);
    EXPECT_EQ(0, mru_entries);

}




