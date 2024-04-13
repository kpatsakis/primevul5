static int ca8210_set_channel(
	struct ieee802154_hw  *hw,
	u8                     page,
	u8                     channel
)
{
	u8 status;
	struct ca8210_priv *priv = hw->priv;

	status = mlme_set_request_sync(
		PHY_CURRENT_CHANNEL,
		0,
		1,
		&channel,
		priv->spi
	);
	if (status) {
		dev_err(
			&priv->spi->dev,
			"error setting channel, MLME-SET.confirm status = %d\n",
			status
		);
	}
	return link_to_linux_err(status);
}