static void hci_le_remote_conn_param_req_evt(struct hci_dev *hdev,
					     struct sk_buff *skb)
{
	struct hci_ev_le_remote_conn_param_req *ev = (void *) skb->data;
	struct hci_cp_le_conn_param_req_reply cp;
	struct hci_conn *hcon;
	u16 handle, min, max, latency, timeout;

	handle = le16_to_cpu(ev->handle);
	min = le16_to_cpu(ev->interval_min);
	max = le16_to_cpu(ev->interval_max);
	latency = le16_to_cpu(ev->latency);
	timeout = le16_to_cpu(ev->timeout);

	hcon = hci_conn_hash_lookup_handle(hdev, handle);
	if (!hcon || hcon->state != BT_CONNECTED)
		return send_conn_param_neg_reply(hdev, handle,
						 HCI_ERROR_UNKNOWN_CONN_ID);

	if (hci_check_conn_params(min, max, latency, timeout))
		return send_conn_param_neg_reply(hdev, handle,
						 HCI_ERROR_INVALID_LL_PARAMS);

	if (hcon->role == HCI_ROLE_MASTER) {
		struct hci_conn_params *params;
		u8 store_hint;

		hci_dev_lock(hdev);

		params = hci_conn_params_lookup(hdev, &hcon->dst,
						hcon->dst_type);
		if (params) {
			params->conn_min_interval = min;
			params->conn_max_interval = max;
			params->conn_latency = latency;
			params->supervision_timeout = timeout;
			store_hint = 0x01;
		} else{
			store_hint = 0x00;
		}

		hci_dev_unlock(hdev);

		mgmt_new_conn_param(hdev, &hcon->dst, hcon->dst_type,
				    store_hint, min, max, latency, timeout);
	}

	cp.handle = ev->handle;
	cp.interval_min = ev->interval_min;
	cp.interval_max = ev->interval_max;
	cp.latency = ev->latency;
	cp.timeout = ev->timeout;
	cp.min_ce_len = 0;
	cp.max_ce_len = 0;

	hci_send_cmd(hdev, HCI_OP_LE_CONN_PARAM_REQ_REPLY, sizeof(cp), &cp);
}