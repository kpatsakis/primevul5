static int nfcmrvl_nci_close(struct nci_dev *ndev)
{
	struct nfcmrvl_private *priv = nci_get_drvdata(ndev);

	if (!test_and_clear_bit(NFCMRVL_NCI_RUNNING, &priv->flags))
		return 0;

	priv->if_ops->nci_close(priv);

	return 0;
}