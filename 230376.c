static void hci_disconn_phylink_complete_evt(struct hci_dev *hdev,
					     struct sk_buff *skb)
{
	struct hci_ev_disconn_phy_link_complete *ev = (void *) skb->data;
	struct hci_conn *hcon;

	BT_DBG("%s status 0x%2.2x", hdev->name, ev->status);

	if (ev->status)
		return;

	hci_dev_lock(hdev);

	hcon = hci_conn_hash_lookup_handle(hdev, ev->phy_handle);
	if (hcon) {
		hcon->state = BT_CLOSED;
		hci_conn_del(hcon);
	}

	hci_dev_unlock(hdev);
}