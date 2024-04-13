static int ca8210_async_xmit_complete(
	struct ieee802154_hw  *hw,
	u8                     msduhandle,
	u8                     status)
{
	struct ca8210_priv *priv = hw->priv;

	if (priv->nextmsduhandle != msduhandle) {
		dev_err(
			&priv->spi->dev,
			"Unexpected msdu_handle on data confirm, Expected %d, got %d\n",
			priv->nextmsduhandle,
			msduhandle
		);
		return -EIO;
	}

	priv->async_tx_pending = false;
	priv->nextmsduhandle++;

	if (status) {
		dev_err(
			&priv->spi->dev,
			"Link transmission unsuccessful, status = %d\n",
			status
		);
		if (status != MAC_TRANSACTION_OVERFLOW) {
			ieee802154_wake_queue(priv->hw);
			return 0;
		}
	}
	ieee802154_xmit_complete(priv->hw, priv->tx_skb, true);

	return 0;
}