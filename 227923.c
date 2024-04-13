static void tcp_ratehalving_spur_to_response(struct sock *sk)
{
	tcp_enter_cwr(sk, 0);
}