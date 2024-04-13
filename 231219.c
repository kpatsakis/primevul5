mwifiex_setup_rates_from_bssdesc(struct mwifiex_private *priv,
				 struct mwifiex_bssdescriptor *bss_desc,
				 u8 *out_rates, u32 *out_rates_size)
{
	u8 card_rates[MWIFIEX_SUPPORTED_RATES];
	u32 card_rates_size;

	/* Copy AP supported rates */
	memcpy(out_rates, bss_desc->supported_rates, MWIFIEX_SUPPORTED_RATES);
	/* Get the STA supported rates */
	card_rates_size = mwifiex_get_active_data_rates(priv, card_rates);
	/* Get the common rates between AP and STA supported rates */
	if (mwifiex_get_common_rates(priv, out_rates, MWIFIEX_SUPPORTED_RATES,
				     card_rates, card_rates_size)) {
		*out_rates_size = 0;
		mwifiex_dbg(priv->adapter, ERROR,
			    "%s: cannot get common rates\n",
			    __func__);
		return -1;
	}

	*out_rates_size =
		min_t(size_t, strlen(out_rates), MWIFIEX_SUPPORTED_RATES);

	return 0;
}