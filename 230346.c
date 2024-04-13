static void read_enc_key_size_complete(struct hci_dev *hdev, u8 status,
				       u16 opcode, struct sk_buff *skb)
{
	const struct hci_rp_read_enc_key_size *rp;
	struct hci_conn *conn;
	u16 handle;

	BT_DBG("%s status 0x%02x", hdev->name, status);

	if (!skb || skb->len < sizeof(*rp)) {
		bt_dev_err(hdev, "invalid read key size response");
		return;
	}

	rp = (void *)skb->data;
	handle = le16_to_cpu(rp->handle);

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, handle);
	if (!conn)
		goto unlock;

	/* While unexpected, the read_enc_key_size command may fail. The most
	 * secure approach is to then assume the key size is 0 to force a
	 * disconnection.
	 */
	if (rp->status) {
		bt_dev_err(hdev, "failed to read key size for handle %u",
			   handle);
		conn->enc_key_size = 0;
	} else {
		conn->enc_key_size = rp->key_size;
	}

	hci_encrypt_cfm(conn, 0);

unlock:
	hci_dev_unlock(hdev);
}