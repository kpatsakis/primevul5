int __hci_req_enable_ext_advertising(struct hci_request *req, u8 instance)
{
	struct hci_dev *hdev = req->hdev;
	struct hci_cp_le_set_ext_adv_enable *cp;
	struct hci_cp_ext_adv_set *adv_set;
	u8 data[sizeof(*cp) + sizeof(*adv_set) * 1];
	struct adv_info *adv_instance;

	if (instance > 0) {
		adv_instance = hci_find_adv_instance(hdev, instance);
		if (!adv_instance)
			return -EINVAL;
	} else {
		adv_instance = NULL;
	}

	cp = (void *) data;
	adv_set = (void *) cp->data;

	memset(cp, 0, sizeof(*cp));

	cp->enable = 0x01;
	cp->num_of_sets = 0x01;

	memset(adv_set, 0, sizeof(*adv_set));

	adv_set->handle = instance;

	/* Set duration per instance since controller is responsible for
	 * scheduling it.
	 */
	if (adv_instance && adv_instance->duration) {
		u16 duration = adv_instance->timeout * MSEC_PER_SEC;

		/* Time = N * 10 ms */
		adv_set->duration = cpu_to_le16(duration / 10);
	}

	hci_req_add(req, HCI_OP_LE_SET_EXT_ADV_ENABLE,
		    sizeof(*cp) + sizeof(*adv_set) * cp->num_of_sets,
		    data);

	return 0;
}