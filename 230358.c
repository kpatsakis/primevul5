static struct hci_conn *__hci_conn_lookup_handle(struct hci_dev *hdev,
						 __u16 handle)
{
	struct hci_chan *chan;

	switch (hdev->dev_type) {
	case HCI_PRIMARY:
		return hci_conn_hash_lookup_handle(hdev, handle);
	case HCI_AMP:
		chan = hci_chan_lookup_handle(hdev, handle);
		if (chan)
			return chan->conn;
		break;
	default:
		bt_dev_err(hdev, "unknown dev_type %d", hdev->dev_type);
		break;
	}

	return NULL;
}