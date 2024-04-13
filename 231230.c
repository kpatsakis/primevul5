void mwifiex_deauthenticate_all(struct mwifiex_adapter *adapter)
{
	struct mwifiex_private *priv;
	int i;

	for (i = 0; i < adapter->priv_num; i++) {
		priv = adapter->priv[i];
		if (priv)
			mwifiex_deauthenticate(priv, NULL);
	}
}