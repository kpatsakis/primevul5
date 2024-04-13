static int __packet_rcv_has_room(const struct packet_sock *po,
				 const struct sk_buff *skb)
{
	const struct sock *sk = &po->sk;
	int ret = ROOM_NONE;

	if (po->prot_hook.func != tpacket_rcv) {
		int rcvbuf = READ_ONCE(sk->sk_rcvbuf);
		int avail = rcvbuf - atomic_read(&sk->sk_rmem_alloc)
				   - (skb ? skb->truesize : 0);

		if (avail > (rcvbuf >> ROOM_POW_OFF))
			return ROOM_NORMAL;
		else if (avail > 0)
			return ROOM_LOW;
		else
			return ROOM_NONE;
	}

	if (po->tp_version == TPACKET_V3) {
		if (__tpacket_v3_has_room(po, ROOM_POW_OFF))
			ret = ROOM_NORMAL;
		else if (__tpacket_v3_has_room(po, 0))
			ret = ROOM_LOW;
	} else {
		if (__tpacket_has_room(po, ROOM_POW_OFF))
			ret = ROOM_NORMAL;
		else if (__tpacket_has_room(po, 0))
			ret = ROOM_LOW;
	}

	return ret;
}