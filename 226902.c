static void sctp_wfree(struct sk_buff *skb)
{
	struct sctp_chunk *chunk = skb_shinfo(skb)->destructor_arg;
	struct sctp_association *asoc = chunk->asoc;
	struct sock *sk = asoc->base.sk;

	sk_mem_uncharge(sk, skb->truesize);
	sk->sk_wmem_queued -= skb->truesize + sizeof(struct sctp_chunk);
	asoc->sndbuf_used -= skb->truesize + sizeof(struct sctp_chunk);
	WARN_ON(refcount_sub_and_test(sizeof(struct sctp_chunk),
				      &sk->sk_wmem_alloc));

	if (chunk->shkey) {
		struct sctp_shared_key *shkey = chunk->shkey;

		/* refcnt == 2 and !list_empty mean after this release, it's
		 * not being used anywhere, and it's time to notify userland
		 * that this shkey can be freed if it's been deactivated.
		 */
		if (shkey->deactivated && !list_empty(&shkey->key_list) &&
		    refcount_read(&shkey->refcnt) == 2) {
			struct sctp_ulpevent *ev;

			ev = sctp_ulpevent_make_authkey(asoc, shkey->key_id,
							SCTP_AUTH_FREE_KEY,
							GFP_KERNEL);
			if (ev)
				asoc->stream.si->enqueue_event(&asoc->ulpq, ev);
		}
		sctp_auth_shkey_release(chunk->shkey);
	}

	sock_wfree(skb);
	sctp_wake_up_waiters(sk, asoc);

	sctp_association_put(asoc);
}