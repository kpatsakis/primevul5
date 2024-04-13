static void hci_check_pending_name(struct hci_dev *hdev, struct hci_conn *conn,
				   bdaddr_t *bdaddr, u8 *name, u8 name_len)
{
	struct discovery_state *discov = &hdev->discovery;
	struct inquiry_entry *e;

	/* Update the mgmt connected state if necessary. Be careful with
	 * conn objects that exist but are not (yet) connected however.
	 * Only those in BT_CONFIG or BT_CONNECTED states can be
	 * considered connected.
	 */
	if (conn &&
	    (conn->state == BT_CONFIG || conn->state == BT_CONNECTED) &&
	    !test_and_set_bit(HCI_CONN_MGMT_CONNECTED, &conn->flags))
		mgmt_device_connected(hdev, conn, 0, name, name_len);

	if (discov->state == DISCOVERY_STOPPED)
		return;

	if (discov->state == DISCOVERY_STOPPING)
		goto discov_complete;

	if (discov->state != DISCOVERY_RESOLVING)
		return;

	e = hci_inquiry_cache_lookup_resolve(hdev, bdaddr, NAME_PENDING);
	/* If the device was not found in a list of found devices names of which
	 * are pending. there is no need to continue resolving a next name as it
	 * will be done upon receiving another Remote Name Request Complete
	 * Event */
	if (!e)
		return;

	list_del(&e->list);
	if (name) {
		e->name_state = NAME_KNOWN;
		mgmt_remote_name(hdev, bdaddr, ACL_LINK, 0x00,
				 e->data.rssi, name, name_len);
	} else {
		e->name_state = NAME_NOT_KNOWN;
	}

	if (hci_resolve_next_name(hdev))
		return;

discov_complete:
	hci_discovery_set_state(hdev, DISCOVERY_STOPPED);
}