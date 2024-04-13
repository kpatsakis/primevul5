static u8 create_instance_scan_rsp_data(struct hci_dev *hdev, u8 instance,
					u8 *ptr)
{
	struct adv_info *adv_instance;
	u32 instance_flags;
	u8 scan_rsp_len = 0;

	adv_instance = hci_find_adv_instance(hdev, instance);
	if (!adv_instance)
		return 0;

	instance_flags = adv_instance->flags;

	if ((instance_flags & MGMT_ADV_FLAG_APPEARANCE) && hdev->appearance) {
		scan_rsp_len = append_appearance(hdev, ptr, scan_rsp_len);
	}

	memcpy(&ptr[scan_rsp_len], adv_instance->scan_rsp_data,
	       adv_instance->scan_rsp_len);

	scan_rsp_len += adv_instance->scan_rsp_len;

	if (instance_flags & MGMT_ADV_FLAG_LOCAL_NAME)
		scan_rsp_len = append_local_name(hdev, ptr, scan_rsp_len);

	return scan_rsp_len;
}