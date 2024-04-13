static bool adv_cur_instance_is_scannable(struct hci_dev *hdev)
{
	return adv_instance_is_scannable(hdev, hdev->cur_adv_instance);
}