static bool adv_instance_is_scannable(struct hci_dev *hdev, u8 instance)
{
	struct adv_info *adv_instance;

	/* Instance 0x00 always set local name */
	if (instance == 0x00)
		return true;

	adv_instance = hci_find_adv_instance(hdev, instance);
	if (!adv_instance)
		return false;

	if (adv_instance->flags & MGMT_ADV_FLAG_APPEARANCE ||
	    adv_instance->flags & MGMT_ADV_FLAG_LOCAL_NAME)
		return true;

	return adv_instance->scan_rsp_len ? true : false;
}