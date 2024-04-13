static void hci_chan_selected_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_channel_selected *ev = (void *)skb->data;
	struct hci_conn *hcon;

	BT_DBG("%s handle 0x%2.2x", hdev->name, ev->phy_handle);

	skb_pull(skb, sizeof(*ev));

	hcon = hci_conn_hash_lookup_handle(hdev, ev->phy_handle);
	if (!hcon)
		return;

	amp_read_loc_assoc_final_data(hdev, hcon);
}