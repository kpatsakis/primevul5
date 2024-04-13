static int nfcmrvl_nci_setup(struct nci_dev *ndev)
{
	__u8 val = 1;

	nci_set_config(ndev, NFCMRVL_PB_BAIL_OUT, 1, &val);
	return 0;
}