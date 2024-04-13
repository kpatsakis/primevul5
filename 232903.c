static struct sock *udp4_lib_lookup2(struct net *net,
		__be32 saddr, __be16 sport,
		__be32 daddr, unsigned int hnum, int dif, bool exact_dif,
		struct udp_hslot *hslot2,
		struct sk_buff *skb)
{
	struct sock *sk, *result;
	int score, badness, matches = 0, reuseport = 0;
	u32 hash = 0;

	result = NULL;
	badness = 0;
	udp_portaddr_for_each_entry_rcu(sk, &hslot2->head) {
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
			badness = score;
			result = sk;
		} else if (score == badness && reuseport) {
			matches++;
			if (reciprocal_scale(hash, matches) == 0)
				result = sk;
			hash = next_pseudo_random32(hash);
		}
	}
	return result;
}