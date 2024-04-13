int nfcmrvl_nci_recv_frame(struct nfcmrvl_private *priv, struct sk_buff *skb)
{
	if (priv->config.hci_muxed) {
		if (skb->data[0] == NFCMRVL_HCI_EVENT_CODE &&
		    skb->data[1] == NFCMRVL_HCI_NFC_EVENT_CODE) {
			/* Data packet, let's extract NCI payload */
			skb_pull(skb, NFCMRVL_HCI_EVENT_HEADER_SIZE);
		} else {
			/* Skip this packet */
			kfree_skb(skb);
			return 0;
		}
	}

	if (priv->ndev->nfc_dev->fw_download_in_progress) {
		nfcmrvl_fw_dnld_recv_frame(priv, skb);
		return 0;
	}

	if (test_bit(NFCMRVL_NCI_RUNNING, &priv->flags))
		nci_recv_frame(priv->ndev, skb);
	else {
		/* Drop this packet since nobody wants it */
		kfree_skb(skb);
		return 0;
	}

	return 0;
}