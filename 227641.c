static int ca8210_xmit_async(struct ieee802154_hw *hw, struct sk_buff *skb)
{
	struct ca8210_priv *priv = hw->priv;
	int status;

	dev_dbg(&priv->spi->dev, "calling %s\n", __func__);

	priv->tx_skb = skb;
	priv->async_tx_pending = true;
	status = ca8210_skb_tx(skb, priv->nextmsduhandle, priv);
	return status;
}