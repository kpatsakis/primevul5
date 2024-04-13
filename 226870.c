static inline int sctp_wspace(struct sctp_association *asoc)
{
	struct sock *sk = asoc->base.sk;

	return asoc->ep->sndbuf_policy ? sk->sk_sndbuf - asoc->sndbuf_used
				       : sk_stream_wspace(sk);
}