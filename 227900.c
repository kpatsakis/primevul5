static void tcp_fixup_rcvbuf(struct sock *sk)
{
	u32 mss = tcp_sk(sk)->advmss;
	u32 icwnd = TCP_DEFAULT_INIT_RCVWND;
	int rcvmem;

	/* Limit to 10 segments if mss <= 1460,
	 * or 14600/mss segments, with a minimum of two segments.
	 */
	if (mss > 1460)
		icwnd = max_t(u32, (1460 * TCP_DEFAULT_INIT_RCVWND) / mss, 2);

	rcvmem = SKB_TRUESIZE(mss + MAX_TCP_HEADER);
	while (tcp_win_from_space(rcvmem) < mss)
		rcvmem += 128;

	rcvmem *= icwnd;

	if (sk->sk_rcvbuf < rcvmem)
		sk->sk_rcvbuf = min(rcvmem, sysctl_tcp_rmem[2]);
}