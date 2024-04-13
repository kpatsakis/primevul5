static inline void mcba_init_ctx(struct mcba_priv *priv)
{
	int i = 0;

	for (i = 0; i < MCBA_MAX_TX_URBS; i++) {
		priv->tx_context[i].ndx = MCBA_CTX_FREE;
		priv->tx_context[i].priv = priv;
	}

	atomic_set(&priv->free_ctx_cnt, ARRAY_SIZE(priv->tx_context));
}