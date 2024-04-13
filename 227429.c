static void adv_timeout_expire(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    adv_instance_expire.work);

	struct hci_request req;
	u8 instance;

	bt_dev_dbg(hdev, "");

	hci_dev_lock(hdev);

	hdev->adv_instance_timeout = 0;

	instance = hdev->cur_adv_instance;
	if (instance == 0x00)
		goto unlock;

	hci_req_init(&req, hdev);

	hci_req_clear_adv_instance(hdev, NULL, &req, instance, false);

	if (list_empty(&hdev->adv_instances))
		__hci_req_disable_advertising(&req);

	hci_req_run(&req, NULL);

unlock:
	hci_dev_unlock(hdev);
}