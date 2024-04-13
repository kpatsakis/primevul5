static void clear_pending_adv_report(struct hci_dev *hdev)
{
	struct discovery_state *d = &hdev->discovery;

	bacpy(&d->last_adv_addr, BDADDR_ANY);
	d->last_adv_data_len = 0;
}