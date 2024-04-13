static void hci_le_ltk_request_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_le_ltk_req *ev = (void *) skb->data;
	struct hci_cp_le_ltk_reply cp;
	struct hci_cp_le_ltk_neg_reply neg;
	struct hci_conn *conn;
	struct smp_ltk *ltk;

	BT_DBG("%s handle 0x%4.4x", hdev->name, __le16_to_cpu(ev->handle));

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(ev->handle));
	if (conn == NULL)
		goto not_found;

	ltk = hci_find_ltk(hdev, &conn->dst, conn->dst_type, conn->role);
	if (!ltk)
		goto not_found;

	if (smp_ltk_is_sc(ltk)) {
		/* With SC both EDiv and Rand are set to zero */
		if (ev->ediv || ev->rand)
			goto not_found;
	} else {
		/* For non-SC keys check that EDiv and Rand match */
		if (ev->ediv != ltk->ediv || ev->rand != ltk->rand)
			goto not_found;
	}

	memcpy(cp.ltk, ltk->val, ltk->enc_size);
	memset(cp.ltk + ltk->enc_size, 0, sizeof(cp.ltk) - ltk->enc_size);
	cp.handle = cpu_to_le16(conn->handle);

	conn->pending_sec_level = smp_ltk_sec_level(ltk);

	conn->enc_key_size = ltk->enc_size;

	hci_send_cmd(hdev, HCI_OP_LE_LTK_REPLY, sizeof(cp), &cp);

	/* Ref. Bluetooth Core SPEC pages 1975 and 2004. STK is a
	 * temporary key used to encrypt a connection following
	 * pairing. It is used during the Encrypted Session Setup to
	 * distribute the keys. Later, security can be re-established
	 * using a distributed LTK.
	 */
	if (ltk->type == SMP_STK) {
		set_bit(HCI_CONN_STK_ENCRYPT, &conn->flags);
		list_del_rcu(&ltk->list);
		kfree_rcu(ltk, rcu);
	} else {
		clear_bit(HCI_CONN_STK_ENCRYPT, &conn->flags);
	}

	hci_dev_unlock(hdev);

	return;

not_found:
	neg.handle = ev->handle;
	hci_send_cmd(hdev, HCI_OP_LE_LTK_NEG_REPLY, sizeof(neg), &neg);
	hci_dev_unlock(hdev);
}