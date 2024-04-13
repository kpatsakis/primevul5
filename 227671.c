static int ca8210_skb_rx(
	struct ieee802154_hw  *hw,
	size_t                 len,
	u8                    *data_ind
)
{
	struct ieee802154_hdr hdr;
	int msdulen;
	int hlen;
	u8 mpdulinkquality = data_ind[23];
	struct sk_buff *skb;
	struct ca8210_priv *priv = hw->priv;

	/* Allocate mtu size buffer for every rx packet */
	skb = dev_alloc_skb(IEEE802154_MTU + sizeof(hdr));
	if (!skb)
		return -ENOMEM;

	skb_reserve(skb, sizeof(hdr));

	msdulen = data_ind[22]; /* msdu_length */
	if (msdulen > IEEE802154_MTU) {
		dev_err(
			&priv->spi->dev,
			"received erroneously large msdu length!\n"
		);
		kfree_skb(skb);
		return -EMSGSIZE;
	}
	dev_dbg(&priv->spi->dev, "skb buffer length = %d\n", msdulen);

	if (priv->promiscuous)
		goto copy_payload;

	/* Populate hdr */
	hdr.sec.level = data_ind[29 + msdulen];
	dev_dbg(&priv->spi->dev, "security level: %#03x\n", hdr.sec.level);
	if (hdr.sec.level > 0) {
		hdr.sec.key_id_mode = data_ind[30 + msdulen];
		memcpy(&hdr.sec.extended_src, &data_ind[31 + msdulen], 8);
		hdr.sec.key_id = data_ind[39 + msdulen];
	}
	hdr.source.mode = data_ind[0];
	dev_dbg(&priv->spi->dev, "srcAddrMode: %#03x\n", hdr.source.mode);
	hdr.source.pan_id = *(u16 *)&data_ind[1];
	dev_dbg(&priv->spi->dev, "srcPanId: %#06x\n", hdr.source.pan_id);
	memcpy(&hdr.source.extended_addr, &data_ind[3], 8);
	hdr.dest.mode = data_ind[11];
	dev_dbg(&priv->spi->dev, "dstAddrMode: %#03x\n", hdr.dest.mode);
	hdr.dest.pan_id = *(u16 *)&data_ind[12];
	dev_dbg(&priv->spi->dev, "dstPanId: %#06x\n", hdr.dest.pan_id);
	memcpy(&hdr.dest.extended_addr, &data_ind[14], 8);

	/* Fill in FC implicitly */
	hdr.fc.type = 1; /* Data frame */
	if (hdr.sec.level)
		hdr.fc.security_enabled = 1;
	else
		hdr.fc.security_enabled = 0;
	if (data_ind[1] != data_ind[12] || data_ind[2] != data_ind[13])
		hdr.fc.intra_pan = 1;
	else
		hdr.fc.intra_pan = 0;
	hdr.fc.dest_addr_mode = hdr.dest.mode;
	hdr.fc.source_addr_mode = hdr.source.mode;

	/* Add hdr to front of buffer */
	hlen = ieee802154_hdr_push(skb, &hdr);

	if (hlen < 0) {
		dev_crit(&priv->spi->dev, "failed to push mac hdr onto skb!\n");
		kfree_skb(skb);
		return hlen;
	}

	skb_reset_mac_header(skb);
	skb->mac_len = hlen;

copy_payload:
	/* Add <msdulen> bytes of space to the back of the buffer */
	/* Copy msdu to skb */
	skb_put_data(skb, &data_ind[29], msdulen);

	ieee802154_rx_irqsafe(hw, skb, mpdulinkquality);
	return 0;
}