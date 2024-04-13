static int nfcmrvl_nci_send(struct nci_dev *ndev, struct sk_buff *skb)
{
	struct nfcmrvl_private *priv = nci_get_drvdata(ndev);

	nfc_info(priv->dev, "send entry, len %d\n", skb->len);

	skb->dev = (void *)ndev;

	if (priv->config.hci_muxed) {
		unsigned char *hdr;
		unsigned char len = skb->len;

		hdr = skb_push(skb, NFCMRVL_HCI_EVENT_HEADER_SIZE);
		hdr[0] = NFCMRVL_HCI_COMMAND_CODE;
		hdr[1] = NFCMRVL_HCI_OGF;
		hdr[2] = NFCMRVL_HCI_OCF;
		hdr[3] = len;
	}

	return priv->if_ops->nci_send(priv, skb);
}