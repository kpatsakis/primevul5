static int tcp_fast_parse_options(const struct sk_buff *skb,
				  const struct tcphdr *th,
				  struct tcp_sock *tp, const u8 **hvpp)
{
	/* In the spirit of fast parsing, compare doff directly to constant
	 * values.  Because equality is used, short doff can be ignored here.
	 */
	if (th->doff == (sizeof(*th) / 4)) {
		tp->rx_opt.saw_tstamp = 0;
		return 0;
	} else if (tp->rx_opt.tstamp_ok &&
		   th->doff == ((sizeof(*th) + TCPOLEN_TSTAMP_ALIGNED) / 4)) {
		if (tcp_parse_aligned_timestamp(tp, th))
			return 1;
	}
	tcp_parse_options(skb, &tp->rx_opt, hvpp, 1);
	return 1;
}