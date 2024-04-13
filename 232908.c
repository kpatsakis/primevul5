struct sock *__udp4_lib_lookup(struct net *net, __be32 saddr,
		__be16 sport, __be32 daddr, __be16 dport,
		int dif, struct udp_table *udptable, struct sk_buff *skb)
{
	struct sock *sk, *result;
	unsigned short hnum = ntohs(dport);
	unsigned int hash2, slot2, slot = udp_hashfn(net, hnum, udptable->mask);
	struct udp_hslot *hslot2, *hslot = &udptable->hash[slot];
	bool exact_dif = udp_lib_exact_dif_match(net, skb);
	int score, badness, matches = 0, reuseport = 0;
	u32 hash = 0;

	if (hslot->count > 10) {
		hash2 = udp4_portaddr_hash(net, daddr, hnum);
		slot2 = hash2 & udptable->mask;
		hslot2 = &udptable->hash2[slot2];
		if (hslot->count < hslot2->count)
			goto begin;

		result = udp4_lib_lookup2(net, saddr, sport,
					  daddr, hnum, dif,
					  exact_dif, hslot2, skb);
		if (!result) {
			unsigned int old_slot2 = slot2;
			hash2 = udp4_portaddr_hash(net, htonl(INADDR_ANY), hnum);
			slot2 = hash2 & udptable->mask;
			/* avoid searching the same slot again. */
			if (unlikely(slot2 == old_slot2))
				return result;

			hslot2 = &udptable->hash2[slot2];
			if (hslot->count < hslot2->count)
				goto begin;

			result = udp4_lib_lookup2(net, saddr, sport,
						  daddr, hnum, dif,
						  exact_dif, hslot2, skb);
		}
		return result;
	}
begin:
	result = NULL;
	badness = 0;
	sk_for_each_rcu(sk, &hslot->head) {
		score = compute_score(sk, net, saddr, sport,
				      daddr, hnum, dif, exact_dif);
		if (score > badness) {
			reuseport = sk->sk_reuseport;
			if (reuseport) {
				hash = udp_ehashfn(net, daddr, hnum,
						   saddr, sport);
				result = reuseport_select_sock(sk, hash, skb,
							sizeof(struct udphdr));
				if (result)
					return result;
				matches = 1;
			}
			result = sk;
			badness = score;
		} else if (score == badness && reuseport) {
			matches++;
			if (reciprocal_scale(hash, matches) == 0)
				result = sk;
			hash = next_pseudo_random32(hash);
		}
	}
	return result;
}