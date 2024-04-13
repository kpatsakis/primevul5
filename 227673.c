static int ca8210_set_promiscuous_mode(struct ieee802154_hw *hw, const bool on)
{
	u8 status;
	struct ca8210_priv *priv = hw->priv;

	status = mlme_set_request_sync(
		MAC_PROMISCUOUS_MODE,
		0,
		1,
		(const void *)&on,
		priv->spi
	);
	if (status) {
		dev_err(
			&priv->spi->dev,
			"error setting promiscuous mode, MLME-SET.confirm status = %d",
			status
		);
	} else {
		priv->promiscuous = on;
	}
	return link_to_linux_err(status);
}