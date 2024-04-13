static int mwifiex_get_common_rates(struct mwifiex_private *priv, u8 *rate1,
				    u32 rate1_size, u8 *rate2, u32 rate2_size)
{
	int ret;
	u8 *ptr = rate1, *tmp;
	u32 i, j;

	tmp = kmemdup(rate1, rate1_size, GFP_KERNEL);
	if (!tmp) {
		mwifiex_dbg(priv->adapter, ERROR, "failed to alloc tmp buf\n");
		return -ENOMEM;
	}

	memset(rate1, 0, rate1_size);

	for (i = 0; i < rate2_size && rate2[i]; i++) {
		for (j = 0; j < rate1_size && tmp[j]; j++) {
			/* Check common rate, excluding the bit for
			   basic rate */
			if ((rate2[i] & 0x7F) == (tmp[j] & 0x7F)) {
				*rate1++ = tmp[j];
				break;
			}
		}
	}

	mwifiex_dbg(priv->adapter, INFO, "info: Tx data rate set to %#x\n",
		    priv->data_rate);

	if (!priv->is_data_rate_auto) {
		while (*ptr) {
			if ((*ptr & 0x7f) == priv->data_rate) {
				ret = 0;
				goto done;
			}
			ptr++;
		}
		mwifiex_dbg(priv->adapter, ERROR,
			    "previously set fixed data rate %#x\t"
			    "is not compatible with the network\n",
			    priv->data_rate);

		ret = -1;
		goto done;
	}

	ret = 0;
done:
	kfree(tmp);
	return ret;
}