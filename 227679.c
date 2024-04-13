static int ca8210_set_csma_params(
	struct ieee802154_hw  *hw,
	u8                     min_be,
	u8                     max_be,
	u8                     retries
)
{
	u8 status;
	struct ca8210_priv *priv = hw->priv;

	status = mlme_set_request_sync(MAC_MIN_BE, 0, 1, &min_be, priv->spi);
	if (status) {
		dev_err(
			&priv->spi->dev,
			"error setting min be, MLME-SET.confirm status = %d",
			status
		);
		return link_to_linux_err(status);
	}
	status = mlme_set_request_sync(MAC_MAX_BE, 0, 1, &max_be, priv->spi);
	if (status) {
		dev_err(
			&priv->spi->dev,
			"error setting max be, MLME-SET.confirm status = %d",
			status
		);
		return link_to_linux_err(status);
	}
	status = mlme_set_request_sync(
		MAC_MAX_CSMA_BACKOFFS,
		0,
		1,
		&retries,
		priv->spi
	);
	if (status) {
		dev_err(
			&priv->spi->dev,
			"error setting max csma backoffs, MLME-SET.confirm status = %d",
			status
		);
	}
	return link_to_linux_err(status);
}