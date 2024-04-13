static bool has_pending_adv_report(struct hci_dev *hdev)
{
	struct discovery_state *d = &hdev->discovery;

	return bacmp(&d->last_adv_addr, BDADDR_ANY);
}