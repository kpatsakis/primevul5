static bool __tpacket_has_room(const struct packet_sock *po, int pow_off)
{
	int idx, len;

	len = READ_ONCE(po->rx_ring.frame_max) + 1;
	idx = READ_ONCE(po->rx_ring.head);
	if (pow_off)
		idx += len >> pow_off;
	if (idx >= len)
		idx -= len;
	return packet_lookup_frame(po, &po->rx_ring, idx, TP_STATUS_KERNEL);
}