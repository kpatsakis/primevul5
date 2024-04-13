static void xennet_make_one_txreq(unsigned long gfn, unsigned int offset,
				  unsigned int len, void *data)
{
	struct xennet_gnttab_make_txreq *info = data;

	info->tx->flags |= XEN_NETTXF_more_data;
	skb_get(info->skb);
	xennet_tx_setup_grant(gfn, offset, len, data);
}