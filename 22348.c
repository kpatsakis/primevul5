static int nfcmrvl_nci_fw_download(struct nci_dev *ndev,
				   const char *firmware_name)
{
	return nfcmrvl_fw_dnld_start(ndev, firmware_name);
}