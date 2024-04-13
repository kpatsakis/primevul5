static void sctp_for_each_rx_skb(struct sctp_association *asoc, struct sock *sk,
				 void (*cb)(struct sk_buff *, struct sock *))

{
	struct sk_buff *skb, *tmp;

	sctp_skb_for_each(skb, &asoc->ulpq.lobby, tmp)
		cb(skb, sk);

	sctp_skb_for_each(skb, &asoc->ulpq.reasm, tmp)
		cb(skb, sk);

	sctp_skb_for_each(skb, &asoc->ulpq.reasm_uo, tmp)
		cb(skb, sk);
}