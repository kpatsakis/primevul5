static void hci_inquiry_complete_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);
	struct discovery_state *discov = &hdev->discovery;
	struct inquiry_entry *e;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	hci_conn_check_pending(hdev);

	if (!test_and_clear_bit(HCI_INQUIRY, &hdev->flags))
		return;

	smp_mb__after_atomic(); /* wake_up_bit advises about this barrier */
	wake_up_bit(&hdev->flags, HCI_INQUIRY);

	if (!hci_dev_test_flag(hdev, HCI_MGMT))
		return;

	hci_dev_lock(hdev);

	if (discov->state != DISCOVERY_FINDING)
		goto unlock;

	if (list_empty(&discov->resolve)) {
		/* When BR/EDR inquiry is active and no LE scanning is in
		 * progress, then change discovery state to indicate completion.
		 *
		 * When running LE scanning and BR/EDR inquiry simultaneously
		 * and the LE scan already finished, then change the discovery
		 * state to indicate completion.
		 */
		if (!hci_dev_test_flag(hdev, HCI_LE_SCAN) ||
		    !test_bit(HCI_QUIRK_SIMULTANEOUS_DISCOVERY, &hdev->quirks))
			hci_discovery_set_state(hdev, DISCOVERY_STOPPED);
		goto unlock;
	}

	e = hci_inquiry_cache_lookup_resolve(hdev, BDADDR_ANY, NAME_NEEDED);
	if (e && hci_resolve_name(hdev, e) == 0) {
		e->name_state = NAME_PENDING;
		hci_discovery_set_state(hdev, DISCOVERY_RESOLVING);
	} else {
		/* When BR/EDR inquiry is active and no LE scanning is in
		 * progress, then change discovery state to indicate completion.
		 *
		 * When running LE scanning and BR/EDR inquiry simultaneously
		 * and the LE scan already finished, then change the discovery
		 * state to indicate completion.
		 */
		if (!hci_dev_test_flag(hdev, HCI_LE_SCAN) ||
		    !test_bit(HCI_QUIRK_SIMULTANEOUS_DISCOVERY, &hdev->quirks))
			hci_discovery_set_state(hdev, DISCOVERY_STOPPED);
	}

unlock:
	hci_dev_unlock(hdev);
}