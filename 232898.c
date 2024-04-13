static int __udp_queue_rcv_skb(struct sock *sk, struct sk_buff *skb)
{
	int rc;

	if (inet_sk(sk)->inet_daddr) {
		sock_rps_save_rxhash(sk, skb);
		sk_mark_napi_id(sk, skb);
		sk_incoming_cpu_update(sk);
	} else {
		sk_mark_napi_id_once(sk, skb);
	}

	/* At recvmsg() time we may access skb->dst or skb->sp depending on
	 * the IP options and the cmsg flags, elsewhere can we clear all
	 * pending head states while they are hot in the cache
	 */
	if (likely(IPCB(skb)->opt.optlen == 0 && !skb_sec_path(skb)))
		skb_release_head_state(skb);

	rc = __udp_enqueue_schedule_skb(sk, skb);
	if (rc < 0) {
		int is_udplite = IS_UDPLITE(sk);

		/* Note that an ENOMEM error is charged twice */
		if (rc == -ENOMEM)
			UDP_INC_STATS(sock_net(sk), UDP_MIB_RCVBUFERRORS,
					is_udplite);
		UDP_INC_STATS(sock_net(sk), UDP_MIB_INERRORS, is_udplite);
		kfree_skb(skb);
		trace_udp_fail_queue_rcv_skb(rc, sk);
		return -1;
	}

	return 0;
}