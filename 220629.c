static void *prb_lookup_block(const struct packet_sock *po,
			      const struct packet_ring_buffer *rb,
			      unsigned int idx,
			      int status)
{
	struct tpacket_kbdq_core *pkc  = GET_PBDQC_FROM_RB(rb);
	struct tpacket_block_desc *pbd = GET_PBLOCK_DESC(pkc, idx);

	if (status != BLOCK_STATUS(pbd))
		return NULL;
	return pbd;
}