static u8 create_instance_adv_data(struct hci_dev *hdev, u8 instance, u8 *ptr)
{
	struct adv_info *adv_instance = NULL;
	u8 ad_len = 0, flags = 0;
	u32 instance_flags;

	/* Return 0 when the current instance identifier is invalid. */
	if (instance) {
		adv_instance = hci_find_adv_instance(hdev, instance);
		if (!adv_instance)
			return 0;
	}

	instance_flags = get_adv_instance_flags(hdev, instance);

	/* If instance already has the flags set skip adding it once
	 * again.
	 */
	if (adv_instance && eir_get_data(adv_instance->adv_data,
					 adv_instance->adv_data_len, EIR_FLAGS,
					 NULL))
		goto skip_flags;

	/* The Add Advertising command allows userspace to set both the general
	 * and limited discoverable flags.
	 */
	if (instance_flags & MGMT_ADV_FLAG_DISCOV)
		flags |= LE_AD_GENERAL;

	if (instance_flags & MGMT_ADV_FLAG_LIMITED_DISCOV)
		flags |= LE_AD_LIMITED;

	if (!hci_dev_test_flag(hdev, HCI_BREDR_ENABLED))
		flags |= LE_AD_NO_BREDR;

	if (flags || (instance_flags & MGMT_ADV_FLAG_MANAGED_FLAGS)) {
		/* If a discovery flag wasn't provided, simply use the global
		 * settings.
		 */
		if (!flags)
			flags |= mgmt_get_adv_discov_flags(hdev);

		/* If flags would still be empty, then there is no need to
		 * include the "Flags" AD field".
		 */
		if (flags) {
			ptr[0] = 0x02;
			ptr[1] = EIR_FLAGS;
			ptr[2] = flags;

			ad_len += 3;
			ptr += 3;
		}
	}

skip_flags:
	if (adv_instance) {
		memcpy(ptr, adv_instance->adv_data,
		       adv_instance->adv_data_len);
		ad_len += adv_instance->adv_data_len;
		ptr += adv_instance->adv_data_len;
	}

	if (instance_flags & MGMT_ADV_FLAG_TX_POWER) {
		s8 adv_tx_power;

		if (ext_adv_capable(hdev)) {
			if (adv_instance)
				adv_tx_power = adv_instance->tx_power;
			else
				adv_tx_power = hdev->adv_tx_power;
		} else {
			adv_tx_power = hdev->adv_tx_power;
		}

		/* Provide Tx Power only if we can provide a valid value for it */
		if (adv_tx_power != HCI_TX_POWER_INVALID) {
			ptr[0] = 0x02;
			ptr[1] = EIR_TX_POWER;
			ptr[2] = (u8)adv_tx_power;

			ad_len += 3;
			ptr += 3;
		}
	}

	return ad_len;
}