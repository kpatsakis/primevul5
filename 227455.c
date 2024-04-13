static u32 get_adv_instance_flags(struct hci_dev *hdev, u8 instance)
{
	u32 flags;
	struct adv_info *adv_instance;

	if (instance == 0x00) {
		/* Instance 0 always manages the "Tx Power" and "Flags"
		 * fields
		 */
		flags = MGMT_ADV_FLAG_TX_POWER | MGMT_ADV_FLAG_MANAGED_FLAGS;

		/* For instance 0, the HCI_ADVERTISING_CONNECTABLE setting
		 * corresponds to the "connectable" instance flag.
		 */
		if (hci_dev_test_flag(hdev, HCI_ADVERTISING_CONNECTABLE))
			flags |= MGMT_ADV_FLAG_CONNECTABLE;

		if (hci_dev_test_flag(hdev, HCI_LIMITED_DISCOVERABLE))
			flags |= MGMT_ADV_FLAG_LIMITED_DISCOV;
		else if (hci_dev_test_flag(hdev, HCI_DISCOVERABLE))
			flags |= MGMT_ADV_FLAG_DISCOV;

		return flags;
	}

	adv_instance = hci_find_adv_instance(hdev, instance);

	/* Return 0 when we got an invalid instance identifier. */
	if (!adv_instance)
		return 0;

	return adv_instance->flags;
}