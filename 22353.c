void nfcmrvl_chip_halt(struct nfcmrvl_private *priv)
{
	if (gpio_is_valid(priv->config.reset_n_io))
		gpio_set_value(priv->config.reset_n_io, 0);
}