static void hci_num_comp_blocks_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_num_comp_blocks *ev = (void *) skb->data;
	int i;

	if (hdev->flow_ctl_mode != HCI_FLOW_CTL_MODE_BLOCK_BASED) {
		bt_dev_err(hdev, "wrong event for mode %d", hdev->flow_ctl_mode);
		return;
	}

	if (skb->len < sizeof(*ev) ||
	    skb->len < struct_size(ev, handles, ev->num_hndl)) {
		BT_DBG("%s bad parameters", hdev->name);
		return;
	}

	BT_DBG("%s num_blocks %d num_hndl %d", hdev->name, ev->num_blocks,
	       ev->num_hndl);

	for (i = 0; i < ev->num_hndl; i++) {
		struct hci_comp_blocks_info *info = &ev->handles[i];
		struct hci_conn *conn = NULL;
		__u16  handle, block_count;

		handle = __le16_to_cpu(info->handle);
		block_count = __le16_to_cpu(info->blocks);

		conn = __hci_conn_lookup_handle(hdev, handle);
		if (!conn)
			continue;

		conn->sent -= block_count;

		switch (conn->type) {
		case ACL_LINK:
		case AMP_LINK:
			hdev->block_cnt += block_count;
			if (hdev->block_cnt > hdev->num_blocks)
				hdev->block_cnt = hdev->num_blocks;
			break;

		default:
			bt_dev_err(hdev, "unknown type %d conn %p",
				   conn->type, conn);
			break;
		}
	}

	queue_work(hdev->workqueue, &hdev->tx_work);
}