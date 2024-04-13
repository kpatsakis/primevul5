static struct xen_netif_tx_request *xennet_make_first_txreq(
	struct xennet_gnttab_make_txreq *info,
	unsigned int offset, unsigned int len)
{
	info->size = 0;

	gnttab_for_one_grant(info->page, offset, len, xennet_tx_setup_grant, info);

	return info->tx;
}