void hci_request_cancel_all(struct hci_dev *hdev)
{
	hci_req_sync_cancel(hdev, ENODEV);

	cancel_work_sync(&hdev->discov_update);
	cancel_work_sync(&hdev->bg_scan_update);
	cancel_work_sync(&hdev->scan_update);
	cancel_work_sync(&hdev->connectable_update);
	cancel_work_sync(&hdev->discoverable_update);
	cancel_delayed_work_sync(&hdev->discov_off);
	cancel_delayed_work_sync(&hdev->le_scan_disable);
	cancel_delayed_work_sync(&hdev->le_scan_restart);

	if (hdev->adv_instance_timeout) {
		cancel_delayed_work_sync(&hdev->adv_instance_expire);
		hdev->adv_instance_timeout = 0;
	}

	cancel_interleave_scan(hdev);
}