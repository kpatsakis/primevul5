static int ca8210_start(struct ieee802154_hw *hw)
{
	int status;
	u8 rx_on_when_idle;
	u8 lqi_threshold = 0;
	struct ca8210_priv *priv = hw->priv;

	priv->last_dsn = -1;
	/* Turn receiver on when idle for now just to test rx */
	rx_on_when_idle = 1;
	status = mlme_set_request_sync(
		MAC_RX_ON_WHEN_IDLE,
		0,
		1,
		&rx_on_when_idle,
		priv->spi
	);
	if (status) {
		dev_crit(
			&priv->spi->dev,
			"Setting rx_on_when_idle failed, status = %d\n",
			status
		);
		return link_to_linux_err(status);
	}
	status = hwme_set_request_sync(
		HWME_LQILIMIT,
		1,
		&lqi_threshold,
		priv->spi
	);
	if (status) {
		dev_crit(
			&priv->spi->dev,
			"Setting lqilimit failed, status = %d\n",
			status
		);
		return link_to_linux_err(status);
	}

	return 0;
}