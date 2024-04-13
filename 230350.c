static void hci_cc_read_flow_control_mode(struct hci_dev *hdev,
					  struct sk_buff *skb)
{
	struct hci_rp_read_flow_control_mode *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	hdev->flow_ctl_mode = rp->mode;
}