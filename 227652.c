static int ca8210_get_ed(struct ieee802154_hw *hw, u8 *level)
{
	u8 lenvar;
	struct ca8210_priv *priv = hw->priv;

	return link_to_linux_err(
		hwme_get_request_sync(HWME_EDVALUE, &lenvar, level, priv->spi)
	);
}