static int __udp4_lib_mcast_deliver(struct net *net, struct sk_buff *skb,
				    struct udphdr  *uh,
				    __be32 saddr, __be32 daddr,
				    struct udp_table *udptable,
				    int proto)
{
	struct sock *sk, *first = NULL;
	unsigned short hnum = ntohs(uh->dest);
	struct udp_hslot *hslot = udp_hashslot(udptable, net, hnum);
	unsigned int hash2 = 0, hash2_any = 0, use_hash2 = (hslot->count > 10);
	unsigned int offset = offsetof(typeof(*sk), sk_node);
	int dif = skb->dev->ifindex;
	struct hlist_node *node;
	struct sk_buff *nskb;

	if (use_hash2) {
		hash2_any = udp4_portaddr_hash(net, htonl(INADDR_ANY), hnum) &
			    udptable->mask;
		hash2 = udp4_portaddr_hash(net, daddr, hnum) & udptable->mask;
start_lookup:
		hslot = &udptable->hash2[hash2];
		offset = offsetof(typeof(*sk), __sk_common.skc_portaddr_node);
	}

	sk_for_each_entry_offset_rcu(sk, node, &hslot->head, offset) {
		if (!__udp_is_mcast_sock(net, sk, uh->dest, daddr,
					 uh->source, saddr, dif, hnum))
			continue;

		if (!first) {
			first = sk;
			continue;
		}
		nskb = skb_clone(skb, GFP_ATOMIC);

		if (unlikely(!nskb)) {
			atomic_inc(&sk->sk_drops);
			__UDP_INC_STATS(net, UDP_MIB_RCVBUFERRORS,
					IS_UDPLITE(sk));
			__UDP_INC_STATS(net, UDP_MIB_INERRORS,
					IS_UDPLITE(sk));
			continue;
		}
		if (udp_queue_rcv_skb(sk, nskb) > 0)
			consume_skb(nskb);
	}

	/* Also lookup *:port if we are using hash2 and haven't done so yet. */
	if (use_hash2 && hash2 != hash2_any) {
		hash2 = hash2_any;
		goto start_lookup;
	}

	if (first) {
		if (udp_queue_rcv_skb(first, skb) > 0)
			consume_skb(skb);
	} else {
		kfree_skb(skb);
		__UDP_INC_STATS(net, UDP_MIB_IGNOREDMULTI,
				proto == IPPROTO_UDPLITE);
	}
	return 0;
}