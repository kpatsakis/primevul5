static void hci_cc_le_read_adv_tx_power(struct hci_dev *hdev,
					struct sk_buff *skb)
{
	struct hci_rp_le_read_adv_tx_power *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	hdev->adv_tx_power = rp->tx_power;
}