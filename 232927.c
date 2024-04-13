int udp_init_sock(struct sock *sk)
{
	skb_queue_head_init(&udp_sk(sk)->reader_queue);
	sk->sk_destruct = udp_destruct_sock;
	return 0;
}