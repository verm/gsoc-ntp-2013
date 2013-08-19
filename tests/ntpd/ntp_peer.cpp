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

associd_t UT_associd;


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
    char            peerhostname[]="testhost";
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

    /* for next test cases use, check clear_all function */
    
    peer->cast_flags |= (MDF_TXONLY_MASK);
    peer->disp = 1;
    peer->flags |= FLAG_XLEAVE;
    peer->flip = 0;
    peer->filter_disp[0] = 1;
    peer->flags &= ~(FLAG_REFCLOCK);    
    peer->leap = LEAP_NOWARNING;
	
	/* for next ut case FindPeerByAssoc */
	UT_associd = peer->associd;
    
}

/* set peer dist address success,
   rely on previous test case, peer has no dstadr before. */
TEST_F(ntpPeerTest, SetPeerNewDistAddr) {  
    sockaddr_u		peeraddr;
    char            addrstr[]="192.168.1.1";
    endpt *	prevdstadr;
    u_int   prevpeercnt;
    struct peer*    peer;
    
    SetSockaddr4(&peeraddr, addrstr);
    peer = findexistingpeer(&peeraddr, NULL, NULL, -1, 0);
    EXPECT_NE((struct peer*)NULL, peer);  

    prevdstadr = peer->dstadr;
    prevpeercnt = loopback_interface->peercnt;

    // set_peerdstadr(peer, loopback_interface);
	

    // EXPECT_EQ(1, loopback_interface->peercnt - prevpeercnt);
    // EXPECT_NE(prevdstadr, peer->dstadr);
}

/* Find peer by associd,
   Need to work with previous test case */
TEST_F(ntpPeerTest, FindPeerByAssoc) {  
    struct peer*    peer;
    
    peer = findpeerbyassoc(UT_associd);

    EXPECT_NE((struct peer*)NULL, peer);
    EXPECT_EQ(loopback_interface, peer->dstadr);
}

/* Score all associations with no zero results */
TEST_F(ntpPeerTest, ScoreAllAndGetNoZero) {  
    struct peer*    peer, *speer;
    int res;
    
    peer = findpeerbyassoc(UT_associd);
    /* make this peer score none zero */
    peer->flash &= ~(PEER_TEST_MASK);
    peer->status = CTL_PST_SEL_SYNCCAND;

    /* set all peer's score with the same score */
	for (speer = peer_list; speer != NULL; speer = speer->p_link){
        speer->flags |= FLAG_PREEMPT;
        peer->flash &= ~(PEER_TEST_MASK);
        peer->status = CTL_PST_SEL_SYNCCAND;
    }   

    res = score_all(peer);

    /* the final score is not zero */
    EXPECT_NE(0, res);
}


/* Score all associations, and the final score is 0 */
TEST_F(ntpPeerTest, ScoreAllAndGetZero) {  
    struct peer*    peer, *speer;
    int res;
    
    peer = findpeerbyassoc(UT_associd);
    /* make this peer score highest */
    peer->flash &= ~(PEER_TEST_MASK);
    peer->status = CTL_PST_SEL_SELCAND;

    /* don't calc all peer's score */
	for (speer = peer_list; speer != NULL; speer = speer->p_link){
        speer->flags &= ~FLAG_PREEMPT;
    }   

    res = score_all(peer);

    EXPECT_EQ(0, res);
}

/* peer_clr_stats - clear peer module statistics counters */
TEST_F(ntpPeerTest, PeerClrStats) {  

    peer_clr_stats();

    EXPECT_EQ(0, findpeer_calls);
    EXPECT_EQ(0, assocpeer_calls);
    EXPECT_EQ(0, peer_allocations);
    EXPECT_EQ(0, peer_demobilizations);
}

/* peer_clr_stats - clear peer module statistics counters */
TEST_F(ntpPeerTest, PeerResetAll) {  
    struct peer*    peer;
    
    peer = findpeerbyassoc(UT_associd);
    peer_reset(peer);
    EXPECT_EQ(0, peer->sent);
    EXPECT_EQ(0, peer->received);
    EXPECT_EQ(0, peer->processed);
    EXPECT_EQ(0, peer->badauth);
    EXPECT_EQ(0, peer->bogusorg);
    EXPECT_EQ(0, peer->oldpkt);
    EXPECT_EQ(0, peer->seldisptoolarge);
    EXPECT_EQ(0, peer->selbroken);
    

    peer_all_reset();
	for (peer = peer_list; peer != NULL; peer = peer->p_link){
        EXPECT_EQ(0, peer->sent);
        EXPECT_EQ(0, peer->received);
        EXPECT_EQ(0, peer->processed);
        EXPECT_EQ(0, peer->badauth);
        EXPECT_EQ(0, peer->bogusorg);
        EXPECT_EQ(0, peer->oldpkt);
        EXPECT_EQ(0, peer->seldisptoolarge);
        EXPECT_EQ(0, peer->selbroken);
    }
}


/* Add a new peer, then clear all peers */
TEST_F(ntpPeerTest, ClearAllPeer) {  
    sockaddr_u		peeraddr;
    char            addrstr[]="127.0.0.1";
    char            peerhostname[]="localhost";
    
    char            pre_addrstr[]="0.0.0.0";
    char            pre_peerhostname[]="testhost";
    struct peer*    peer;
    
    SetSockaddr4(&peeraddr, addrstr);

    /* add a new peer with MDF_TXONLY_MASK */
    peer = peer_config(&peeraddr,
				NULL,
				loopback_interface,
				MODE_CLIENT,
				NTP_VERSION,
				NTP_MINDPOLL,
				NTP_MAXDPOLL,
				0, /* peerflags */
				0, /* ttl */
				0, /* peerkey */
				NULL /* group ident */);

    peer->cast_flags &= ~(MDF_TXONLY_MASK);

    clear_all();

    /* check the peer with the MDF_TXONLY_MASK */
    EXPECT_NE(peer->ppoll, peer->maxpoll);    
    EXPECT_EQ(peer->hpoll, peer->minpoll);
    EXPECT_NE(peer->disp, MAXDISPERSE);

    /* check a peer without the MDF_TXONLY_MASK */ 
    SetSockaddr4(&peeraddr, pre_addrstr);
    peer = findexistingpeer(&peeraddr, pre_peerhostname, NULL, -1, 0);
    EXPECT_NE((struct peer*)NULL, peer); 
    EXPECT_NE(peer->disp, MAXDISPERSE);
    EXPECT_NE(peer->flip, 1);
    EXPECT_NE(peer->filter_disp[0], MAXDISPERSE);
    EXPECT_NE(peer->leap, LEAP_NOTINSYNC);

    /* refresh the peers, in order to avoid faults */
    refresh_all_peerinterfaces();
}

/* unpeer function: remove peer structure from hash table and free structure */
TEST_F(ntpPeerTest, Unpeer) {  
    struct peer*    peer, *speer;
    int res;
    u_long pre_peer_demobilizations;
    u_long pre_peer_associations;
    int pre_peer_preempt;
    int pre_peer_free_count;
    
    peer = findpeerbyassoc(UT_associd);
    peer->flags |= FLAG_PREEMPT;

    pre_peer_demobilizations = peer_demobilizations;
    pre_peer_associations = peer_associations;
    pre_peer_preempt = peer_preempt;
    pre_peer_free_count = peer_free_count;
    
    unpeer(peer);

    EXPECT_EQ((endpt *)NULL, peer->dstadr);
    EXPECT_EQ(1, peer_demobilizations - pre_peer_demobilizations );
    EXPECT_EQ(1, pre_peer_associations - peer_associations );
    EXPECT_EQ(1, pre_peer_preempt - peer_preempt );
    EXPECT_EQ(1, peer_free_count - pre_peer_free_count );
}


