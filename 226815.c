static inline void sctp_set_owner_w(struct sctp_chunk *chunk)
{
	struct sctp_association *asoc = chunk->asoc;
	struct sock *sk = asoc->base.sk;

	/* The sndbuf space is tracked per association.  */
	sctp_association_hold(asoc);

	if (chunk->shkey)
		sctp_auth_shkey_hold(chunk->shkey);

	skb_set_owner_w(chunk->skb, sk);

	chunk->skb->destructor = sctp_wfree;
	/* Save the chunk pointer in skb for sctp_wfree to use later.  */
	skb_shinfo(chunk->skb)->destructor_arg = chunk;

	refcount_add(sizeof(struct sctp_chunk), &sk->sk_wmem_alloc);
	asoc->sndbuf_used += chunk->skb->truesize + sizeof(struct sctp_chunk);
	sk->sk_wmem_queued += chunk->skb->truesize + sizeof(struct sctp_chunk);
	sk_mem_charge(sk, chunk->skb->truesize);
}