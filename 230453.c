static void hci_le_ext_adv_term_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_evt_le_ext_adv_set_term *ev = (void *) skb->data;
	struct hci_conn *conn;

	BT_DBG("%s status 0x%2.2x", hdev->name, ev->status);

	if (ev->status)
		return;

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(ev->conn_handle));
	if (conn) {
		struct adv_info *adv_instance;

		if (hdev->adv_addr_type != ADDR_LE_DEV_RANDOM)
			return;

		if (!hdev->cur_adv_instance) {
			bacpy(&conn->resp_addr, &hdev->random_addr);
			return;
		}

		adv_instance = hci_find_adv_instance(hdev, hdev->cur_adv_instance);
		if (adv_instance)
			bacpy(&conn->resp_addr, &adv_instance->random_addr);
	}
}