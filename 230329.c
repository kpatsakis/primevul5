static bool hci_get_cmd_complete(struct hci_dev *hdev, u16 opcode,
				 u8 event, struct sk_buff *skb)
{
	struct hci_ev_cmd_complete *ev;
	struct hci_event_hdr *hdr;

	if (!skb)
		return false;

	if (skb->len < sizeof(*hdr)) {
		bt_dev_err(hdev, "too short HCI event");
		return false;
	}

	hdr = (void *) skb->data;
	skb_pull(skb, HCI_EVENT_HDR_SIZE);

	if (event) {
		if (hdr->evt != event)
			return false;
		return true;
	}

	/* Check if request ended in Command Status - no way to retreive
	 * any extra parameters in this case.
	 */
	if (hdr->evt == HCI_EV_CMD_STATUS)
		return false;

	if (hdr->evt != HCI_EV_CMD_COMPLETE) {
		bt_dev_err(hdev, "last event is not cmd complete (0x%2.2x)",
			   hdr->evt);
		return false;
	}

	if (skb->len < sizeof(*ev)) {
		bt_dev_err(hdev, "too short cmd_complete event");
		return false;
	}

	ev = (void *) skb->data;
	skb_pull(skb, sizeof(*ev));

	if (opcode != __le16_to_cpu(ev->opcode)) {
		BT_DBG("opcode doesn't match (0x%2.2x != 0x%2.2x)", opcode,
		       __le16_to_cpu(ev->opcode));
		return false;
	}

	return true;
}