/* ntp_peer.cpp - management of data maintained for peer associations */
#include "ntpdtest.h"
     
 /* these following segments are copied from ntp_monitor.c */
 extern "C" {
#include <stdio.h>
#include <sys/types.h>
    
#include "ntpd.h"
#include "ntp_lists.h"
#include "ntp_stdlib.h"
#include "ntp_control.h"
#include <ntp_random.h>
#include <ntp_net.h>
};

class ntpPeerTest : public ntpdtest {
public:
	void SetSockaddr4(sockaddr_u* s, const char* address) {
		s->sa4.sin_family = AF_INET;
		s->sa4.sin_addr.s_addr = inet_addr(address);
		SET_PORT(s, 123);
        return;
	}
protected:
	virtual void SetUp() {
	}

};

/* Confirm init_peer will set global vars */
TEST_F(ntpPeerTest, InitPeer) {  
    total_peer_structs = 0;
    peer_free_count = 0;

    init_peer();

    /* In fact 8 should be replaced with COUNTOF(init_peer_alloc),
       But the var is a static var and no interface to read its size */
    EXPECT_EQ(8, total_peer_structs);
    EXPECT_EQ(8, peer_free_count);
}

/* Find a peer that do not exist by hostname */
TEST_F(ntpPeerTest, FindNoExistPeerByName) {  
    sockaddr_u		peeraddr;
    char            addrstr[]="192.168.1.1";
    char            peerhostname[]="localhost";
    struct peer*    peer;
    
    SetSockaddr4(&peeraddr, addrstr);

    peer = findexistingpeer(&peeraddr, peerhostname, NULL, -1, 0);

    EXPECT_EQ((struct peer*)NULL, peer);    
}

/* Find a peer that do not exist by addr */
TEST_F(ntpPeerTest, FindNoExistPeerByAddr) {  
    sockaddr_u		peeraddr;
    char            addrstr[]="192.168.1.1";
    struct peer*    peer;
    
    SetSockaddr4(&peeraddr, addrstr);

    peer = findexistingpeer(&peeraddr, NULL, NULL, -1, 0);

    EXPECT_EQ((struct peer*)NULL, peer);    
}

/* Add a peer that do not exist then we can find it */
TEST_F(ntpPeerTest, FindExistPeerByAddr) {  
    sockaddr_u		peeraddr;
    char            addrstr[]="192.168.1.1";
    struct peer*    peer;
    
    SetSockaddr4(&peeraddr, addrstr);

    peer_config(&peeraddr,
				NULL,  /* NULL if not set MDF_POOL */
				NULL,
				MODE_CLIENT,
				NTP_VERSION,
				NTP_MINDPOLL,
				NTP_MAXDPOLL,
				0, /* peerflags, or 0 */
				0, /* ttl */
				0, /* peerkey */
				NULL /* group ident */);

    peer = findexistingpeer(&peeraddr, NULL, NULL, -1, 0);

    EXPECT_NE((struct peer*)NULL, peer);    
}

/* Find a peer successfully by name */
TEST_F(ntpPeerTest, FindExistPeerByName) {  
    sockaddr_u		peeraddr;
    char            addrstr[]="0.0.0.0";
    char            peerhostname[]="localhost";
    struct peer*    peer;
    
    SetSockaddr4(&peeraddr, addrstr);

    peer_config(&peeraddr,
				peerhostname,
				loopback_interface,
				MODE_CLIENT,
				NTP_VERSION,
				NTP_MINDPOLL,
				NTP_MAXDPOLL,
				0, /* peerflags */
				0, /* ttl */
				0, /* peerkey */
				NULL /* group ident */);

    peer = findexistingpeer(&peeraddr, peerhostname, NULL, -1, 0);

    EXPECT_NE((struct peer*)NULL, peer);    
}

