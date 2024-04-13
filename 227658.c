static int ca8210_set_cca_mode(
	struct ieee802154_hw       *hw,
	const struct wpan_phy_cca  *cca
)
{
	u8 status;
	u8 cca_mode;
	struct ca8210_priv *priv = hw->priv;

	cca_mode = cca->mode & 3;
	if (cca_mode == 3 && cca->opt == NL802154_CCA_OPT_ENERGY_CARRIER_OR) {
		/* cca_mode 0 == CS OR ED, 3 == CS AND ED */
		cca_mode = 0;
	}
	status = mlme_set_request_sync(
		PHY_CCA_MODE,
		0,
		1,
		&cca_mode,
		priv->spi
	);
	if (status) {
		dev_err(
			&priv->spi->dev,
			"error setting cca mode, MLME-SET.confirm status = %d",
			status
		);
	}
	return link_to_linux_err(status);
}