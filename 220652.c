static bool __tpacket_v3_has_room(const struct packet_sock *po, int pow_off)
{
	int idx, len;

	len = READ_ONCE(po->rx_ring.prb_bdqc.knum_blocks);
	idx = READ_ONCE(po->rx_ring.prb_bdqc.kactive_blk_num);
	if (pow_off)
		idx += len >> pow_off;
	if (idx >= len)
		idx -= len;
	return prb_lookup_block(po, &po->rx_ring, idx, TP_STATUS_KERNEL);
}