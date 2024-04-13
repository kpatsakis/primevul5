int __hci_req_setup_ext_adv_instance(struct hci_request *req, u8 instance)
{
	struct hci_cp_le_set_ext_adv_params cp;
	struct hci_dev *hdev = req->hdev;
	bool connectable;
	u32 flags;
	bdaddr_t random_addr;
	u8 own_addr_type;
	int err;
	struct adv_info *adv_instance;
	bool secondary_adv;

	if (instance > 0) {
		adv_instance = hci_find_adv_instance(hdev, instance);
		if (!adv_instance)
			return -EINVAL;
	} else {
		adv_instance = NULL;
	}

	flags = get_adv_instance_flags(hdev, instance);

	/* If the "connectable" instance flag was not set, then choose between
	 * ADV_IND and ADV_NONCONN_IND based on the global connectable setting.
	 */
	connectable = (flags & MGMT_ADV_FLAG_CONNECTABLE) ||
		      mgmt_get_connectable(hdev);

	if (!is_advertising_allowed(hdev, connectable))
		return -EPERM;

	/* Set require_privacy to true only when non-connectable
	 * advertising is used. In that case it is fine to use a
	 * non-resolvable private address.
	 */
	err = hci_get_random_address(hdev, !connectable,
				     adv_use_rpa(hdev, flags), adv_instance,
				     &own_addr_type, &random_addr);
	if (err < 0)
		return err;

	memset(&cp, 0, sizeof(cp));

	if (adv_instance) {
		hci_cpu_to_le24(adv_instance->min_interval, cp.min_interval);
		hci_cpu_to_le24(adv_instance->max_interval, cp.max_interval);
		cp.tx_power = adv_instance->tx_power;
	} else {
		hci_cpu_to_le24(hdev->le_adv_min_interval, cp.min_interval);
		hci_cpu_to_le24(hdev->le_adv_max_interval, cp.max_interval);
		cp.tx_power = HCI_ADV_TX_POWER_NO_PREFERENCE;
	}

	secondary_adv = (flags & MGMT_ADV_FLAG_SEC_MASK);

	if (connectable) {
		if (secondary_adv)
			cp.evt_properties = cpu_to_le16(LE_EXT_ADV_CONN_IND);
		else
			cp.evt_properties = cpu_to_le16(LE_LEGACY_ADV_IND);
	} else if (adv_instance_is_scannable(hdev, instance)) {
		if (secondary_adv)
			cp.evt_properties = cpu_to_le16(LE_EXT_ADV_SCAN_IND);
		else
			cp.evt_properties = cpu_to_le16(LE_LEGACY_ADV_SCAN_IND);
	} else {
		if (secondary_adv)
			cp.evt_properties = cpu_to_le16(LE_EXT_ADV_NON_CONN_IND);
		else
			cp.evt_properties = cpu_to_le16(LE_LEGACY_NONCONN_IND);
	}

	cp.own_addr_type = own_addr_type;
	cp.channel_map = hdev->le_adv_channel_map;
	cp.handle = instance;

	if (flags & MGMT_ADV_FLAG_SEC_2M) {
		cp.primary_phy = HCI_ADV_PHY_1M;
		cp.secondary_phy = HCI_ADV_PHY_2M;
	} else if (flags & MGMT_ADV_FLAG_SEC_CODED) {
		cp.primary_phy = HCI_ADV_PHY_CODED;
		cp.secondary_phy = HCI_ADV_PHY_CODED;
	} else {
		/* In all other cases use 1M */
		cp.primary_phy = HCI_ADV_PHY_1M;
		cp.secondary_phy = HCI_ADV_PHY_1M;
	}

	hci_req_add(req, HCI_OP_LE_SET_EXT_ADV_PARAMS, sizeof(cp), &cp);

	if (own_addr_type == ADDR_LE_DEV_RANDOM &&
	    bacmp(&random_addr, BDADDR_ANY)) {
		struct hci_cp_le_set_adv_set_rand_addr cp;

		/* Check if random address need to be updated */
		if (adv_instance) {
			if (!bacmp(&random_addr, &adv_instance->random_addr))
				return 0;
		} else {
			if (!bacmp(&random_addr, &hdev->random_addr))
				return 0;
		}

		memset(&cp, 0, sizeof(cp));

		cp.handle = instance;
		bacpy(&cp.bdaddr, &random_addr);

		hci_req_add(req,
			    HCI_OP_LE_SET_ADV_SET_RAND_ADDR,
			    sizeof(cp), &cp);
	}

	return 0;
}