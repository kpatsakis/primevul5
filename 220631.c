static int packet_rcv_has_room(struct packet_sock *po, struct sk_buff *skb)
{
	int pressure, ret;

	ret = __packet_rcv_has_room(po, skb);
	pressure = ret != ROOM_NORMAL;

	if (READ_ONCE(po->pressure) != pressure)
		WRITE_ONCE(po->pressure, pressure);

	return ret;
}