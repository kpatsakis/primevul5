static int ca8210_skb_tx(
	struct sk_buff      *skb,
	u8                   msduhandle,
	struct ca8210_priv  *priv
)
{
	int status;
	struct ieee802154_hdr header = { };
	struct secspec secspec;
	unsigned int mac_len;

	dev_dbg(&priv->spi->dev, "%s called\n", __func__);

	/* Get addressing info from skb - ieee802154 layer creates a full
	 * packet
	 */
	mac_len = ieee802154_hdr_peek_addrs(skb, &header);

	secspec.security_level = header.sec.level;
	secspec.key_id_mode = header.sec.key_id_mode;
	if (secspec.key_id_mode == 2)
		memcpy(secspec.key_source, &header.sec.short_src, 4);
	else if (secspec.key_id_mode == 3)
		memcpy(secspec.key_source, &header.sec.extended_src, 8);
	secspec.key_index = header.sec.key_id;

	/* Pass to Cascoda API */
	status =  mcps_data_request(
		header.source.mode,
		header.dest.mode,
		header.dest.pan_id,
		(union macaddr *)&header.dest.extended_addr,
		skb->len - mac_len,
		&skb->data[mac_len],
		msduhandle,
		header.fc.ack_request,
		&secspec,
		priv->spi
	);
	return link_to_linux_err(status);
}