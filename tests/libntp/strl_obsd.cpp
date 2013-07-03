/* This file tests functions in strl_obsd.c */

#include "libntptest.h"

extern "C" {
/* #include <config.h>		/* + marks local changes */
#ifdef HAVE_SYS_TYPES_H		/* + */
#include <sys/types.h>
#endif				/* + */
#include <string.h>
    
#include "ntp_stdlib.h"		/* + strlcpy, strlcat prototypes */
};

class strl_obsdTest : public libntptest {
protected:
};

TEST_F(strl_obsdTest, strlcpySuccess) {
#ifndef HAVE_STRLCPY
    size_t res;
    char   src[] = "test_string";
    char   dist[16];
    
    res = strlcpy(dist, src, sizeof(dist));

    EXPECT_EQ(res, strlen(src));
    EXPECT_EQ(0, strcmp(src, dist));    
#endif
}

TEST_F(strl_obsdTest, strlcpyNoEnoughSpace) {
#ifndef HAVE_STRLCPY
    size_t res;
    char   src[] = "test_string";
    char   dist[4];
    
    res = strlcpy(dist, src, sizeof(dist));

    EXPECT_EQ(res, strlen(src));
    EXPECT_EQ(0, strncmp(src, dist, sizeof(dist) - 1));    
    EXPECT_EQ(0, dist[sizeof(dist) - 1]);
#endif
}

TEST_F(strl_obsdTest, strlcatSuccess) {
#ifndef HAVE_STRLCPY
    size_t res;
    char   src[] = "test cat";
    char   dist[16] = "start";
    int    predistlen;
    
    predistlen = strlen(dist);
    res = strlcat(dist, src, sizeof(dist));

    EXPECT_EQ(res, strlen(dist));
    EXPECT_EQ(0, strncmp(dist + predistlen,
                         src, strlen(src) + 1));
#endif
}

TEST_F(strl_obsdTest, strlcatNoEnoughSpace) {
#ifndef HAVE_STRLCPY
    size_t res;
    char   src[] = "test cat";
    char   dist[8] = "start";
    int    predistlen;

    predistlen = strlen(dist);
    res = strlcat(dist, src, sizeof(dist));

    EXPECT_EQ(res, predistlen + strlen(src));
    EXPECT_EQ(0, strncmp(dist + predistlen,
                         src, sizeof(dist) - predistlen - 1));
#endif
}



