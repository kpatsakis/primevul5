static int ca8210_set_cca_ed_level(struct ieee802154_hw *hw, s32 level)
{
	u8 status;
	u8 ed_threshold = (level / 100) * 2 + 256;
	struct ca8210_priv *priv = hw->priv;

	status = hwme_set_request_sync(
		HWME_EDTHRESHOLD,
		1,
		&ed_threshold,
		priv->spi
	);
	if (status) {
		dev_err(
			&priv->spi->dev,
			"error setting ed threshold, HWME-SET.confirm status = %d",
			status
		);
	}
	return link_to_linux_err(status);
}