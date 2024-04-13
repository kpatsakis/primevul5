static int ca8210_set_hw_addr_filt(
	struct ieee802154_hw            *hw,
	struct ieee802154_hw_addr_filt  *filt,
	unsigned long                    changed
)
{
	u8 status = 0;
	struct ca8210_priv *priv = hw->priv;

	if (changed & IEEE802154_AFILT_PANID_CHANGED) {
		status = mlme_set_request_sync(
			MAC_PAN_ID,
			0,
			2,
			&filt->pan_id, priv->spi
		);
		if (status) {
			dev_err(
				&priv->spi->dev,
				"error setting pan id, MLME-SET.confirm status = %d",
				status
			);
			return link_to_linux_err(status);
		}
	}
	if (changed & IEEE802154_AFILT_SADDR_CHANGED) {
		status = mlme_set_request_sync(
			MAC_SHORT_ADDRESS,
			0,
			2,
			&filt->short_addr, priv->spi
		);
		if (status) {
			dev_err(
				&priv->spi->dev,
				"error setting short address, MLME-SET.confirm status = %d",
				status
			);
			return link_to_linux_err(status);
		}
	}
	if (changed & IEEE802154_AFILT_IEEEADDR_CHANGED) {
		status = mlme_set_request_sync(
			NS_IEEE_ADDRESS,
			0,
			8,
			&filt->ieee_addr,
			priv->spi
		);
		if (status) {
			dev_err(
				&priv->spi->dev,
				"error setting ieee address, MLME-SET.confirm status = %d",
				status
			);
			return link_to_linux_err(status);
		}
	}
	/* TODO: Should use MLME_START to set coord bit? */
	return 0;
}