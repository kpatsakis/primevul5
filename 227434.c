void hci_request_setup(struct hci_dev *hdev)
{
	INIT_WORK(&hdev->discov_update, discov_update);
	INIT_WORK(&hdev->bg_scan_update, bg_scan_update);
	INIT_WORK(&hdev->scan_update, scan_update_work);
	INIT_WORK(&hdev->connectable_update, connectable_update_work);
	INIT_WORK(&hdev->discoverable_update, discoverable_update_work);
	INIT_DELAYED_WORK(&hdev->discov_off, discov_off);
	INIT_DELAYED_WORK(&hdev->le_scan_disable, le_scan_disable_work);
	INIT_DELAYED_WORK(&hdev->le_scan_restart, le_scan_restart_work);
	INIT_DELAYED_WORK(&hdev->adv_instance_expire, adv_timeout_expire);
	INIT_DELAYED_WORK(&hdev->interleave_scan, interleave_scan_work);
}