static int first_packet_length(struct sock *sk)
{
	struct sk_buff_head *rcvq = &udp_sk(sk)->reader_queue;
	struct sk_buff_head *sk_queue = &sk->sk_receive_queue;
	struct sk_buff *skb;
	int total = 0;
	int res;

	spin_lock_bh(&rcvq->lock);
	skb = __first_packet_length(sk, rcvq, &total);
	if (!skb && !skb_queue_empty(sk_queue)) {
		spin_lock(&sk_queue->lock);
		skb_queue_splice_tail_init(sk_queue, rcvq);
		spin_unlock(&sk_queue->lock);

		skb = __first_packet_length(sk, rcvq, &total);
	}
	res = skb ? skb->len : -1;
	if (total)
		udp_rmem_release(sk, total, 1, false);
	spin_unlock_bh(&rcvq->lock);
	return res;
}