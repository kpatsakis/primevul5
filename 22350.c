void nfcmrvl_chip_reset(struct nfcmrvl_private *priv)
{
	/* Reset possible fault of previous session */
	clear_bit(NFCMRVL_PHY_ERROR, &priv->flags);

	if (gpio_is_valid(priv->config.reset_n_io)) {
		nfc_info(priv->dev, "reset the chip\n");
		gpio_set_value(priv->config.reset_n_io, 0);
		usleep_range(5000, 10000);
		gpio_set_value(priv->config.reset_n_io, 1);
	} else
		nfc_info(priv->dev, "no reset available on this interface\n");
}