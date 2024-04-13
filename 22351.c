static int nfcmrvl_nci_open(struct nci_dev *ndev)
{
	struct nfcmrvl_private *priv = nci_get_drvdata(ndev);
	int err;

	if (test_and_set_bit(NFCMRVL_NCI_RUNNING, &priv->flags))
		return 0;

	/* Reset possible fault of previous session */
	clear_bit(NFCMRVL_PHY_ERROR, &priv->flags);

	err = priv->if_ops->nci_open(priv);

	if (err)
		clear_bit(NFCMRVL_NCI_RUNNING, &priv->flags);

	return err;
}