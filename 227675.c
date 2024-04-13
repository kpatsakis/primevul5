static int ca8210_set_frame_retries(struct ieee802154_hw *hw, s8 retries)
{
	u8 status;
	struct ca8210_priv *priv = hw->priv;

	status = mlme_set_request_sync(
		MAC_MAX_FRAME_RETRIES,
		0,
		1,
		&retries,
		priv->spi
	);
	if (status) {
		dev_err(
			&priv->spi->dev,
			"error setting frame retries, MLME-SET.confirm status = %d",
			status
		);
	}
	return link_to_linux_err(status);
}