static void hci_phy_link_complete_evt(struct hci_dev *hdev,
				      struct sk_buff *skb)
{
	struct hci_ev_phy_link_complete *ev = (void *) skb->data;
	struct hci_conn *hcon, *bredr_hcon;

	BT_DBG("%s handle 0x%2.2x status 0x%2.2x", hdev->name, ev->phy_handle,
	       ev->status);

	hci_dev_lock(hdev);

	hcon = hci_conn_hash_lookup_handle(hdev, ev->phy_handle);
	if (!hcon) {
		hci_dev_unlock(hdev);
		return;
	}

	if (ev->status) {
		hci_conn_del(hcon);
		hci_dev_unlock(hdev);
		return;
	}

	bredr_hcon = hcon->amp_mgr->l2cap_conn->hcon;

	hcon->state = BT_CONNECTED;
	bacpy(&hcon->dst, &bredr_hcon->dst);

	hci_conn_hold(hcon);
	hcon->disc_timeout = HCI_DISCONN_TIMEOUT;
	hci_conn_drop(hcon);

	hci_debugfs_create_conn(hcon);
	hci_conn_add_sysfs(hcon);

	amp_physical_cfm(bredr_hcon, hcon);

	hci_dev_unlock(hdev);
}