static void discov_update(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    discov_update);
	u8 status = 0;

	switch (hdev->discovery.state) {
	case DISCOVERY_STARTING:
		start_discovery(hdev, &status);
		mgmt_start_discovery_complete(hdev, status);
		if (status)
			hci_discovery_set_state(hdev, DISCOVERY_STOPPED);
		else
			hci_discovery_set_state(hdev, DISCOVERY_FINDING);
		break;
	case DISCOVERY_STOPPING:
		hci_req_sync(hdev, stop_discovery, 0, HCI_CMD_TIMEOUT, &status);
		mgmt_stop_discovery_complete(hdev, status);
		if (!status)
			hci_discovery_set_state(hdev, DISCOVERY_STOPPED);
		break;
	case DISCOVERY_STOPPED:
	default:
		return;
	}
}