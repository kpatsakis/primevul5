static void hci_le_enh_conn_complete_evt(struct hci_dev *hdev,
					 struct sk_buff *skb)
{
	struct hci_ev_le_enh_conn_complete *ev = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, ev->status);

	le_conn_complete_evt(hdev, ev->status, &ev->bdaddr, ev->bdaddr_type,
			     ev->role, le16_to_cpu(ev->handle),
			     le16_to_cpu(ev->interval),
			     le16_to_cpu(ev->latency),
			     le16_to_cpu(ev->supervision_timeout));
}