static void tcp_undo_spur_to_response(struct sock *sk, int flag)
{
	if (flag & FLAG_ECE)
		tcp_ratehalving_spur_to_response(sk);
	else
		tcp_undo_cwr(sk, true);
}