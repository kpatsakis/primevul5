static int ca8210_set_tx_power(struct ieee802154_hw *hw, s32 mbm)
{
	struct ca8210_priv *priv = hw->priv;

	mbm /= 100;
	return link_to_linux_err(
		mlme_set_request_sync(PHY_TRANSMIT_POWER, 0, 1, &mbm, priv->spi)
	);
}