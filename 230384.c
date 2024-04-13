static void hci_cc_read_local_version(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_rp_read_local_version *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	if (hci_dev_test_flag(hdev, HCI_SETUP) ||
	    hci_dev_test_flag(hdev, HCI_CONFIG)) {
		hdev->hci_ver = rp->hci_ver;
		hdev->hci_rev = __le16_to_cpu(rp->hci_rev);
		hdev->lmp_ver = rp->lmp_ver;
		hdev->manufacturer = __le16_to_cpu(rp->manufacturer);
		hdev->lmp_subver = __le16_to_cpu(rp->lmp_subver);
	}
}