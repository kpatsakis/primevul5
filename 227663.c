static int ca8210_net_rx(struct ieee802154_hw *hw, u8 *command, size_t len)
{
	struct ca8210_priv *priv = hw->priv;
	unsigned long flags;
	u8 status;

	dev_dbg(&priv->spi->dev, "%s: CmdID = %d\n", __func__, command[0]);

	if (command[0] == SPI_MCPS_DATA_INDICATION) {
		/* Received data */
		spin_lock_irqsave(&priv->lock, flags);
		if (command[26] == priv->last_dsn) {
			dev_dbg(
				&priv->spi->dev,
				"DSN %d resend received, ignoring...\n",
				command[26]
			);
			spin_unlock_irqrestore(&priv->lock, flags);
			return 0;
		}
		priv->last_dsn = command[26];
		spin_unlock_irqrestore(&priv->lock, flags);
		return ca8210_skb_rx(hw, len - 2, command + 2);
	} else if (command[0] == SPI_MCPS_DATA_CONFIRM) {
		status = command[3];
		if (priv->async_tx_pending) {
			return ca8210_async_xmit_complete(
				hw,
				command[2],
				status
			);
		}
	}

	return 0;
}