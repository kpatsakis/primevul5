void hci_req_clear_adv_instance(struct hci_dev *hdev, struct sock *sk,
				struct hci_request *req, u8 instance,
				bool force)
{
	struct adv_info *adv_instance, *n, *next_instance = NULL;
	int err;
	u8 rem_inst;

	/* Cancel any timeout concerning the removed instance(s). */
	if (!instance || hdev->cur_adv_instance == instance)
		cancel_adv_timeout(hdev);

	/* Get the next instance to advertise BEFORE we remove
	 * the current one. This can be the same instance again
	 * if there is only one instance.
	 */
	if (instance && hdev->cur_adv_instance == instance)
		next_instance = hci_get_next_instance(hdev, instance);

	if (instance == 0x00) {
		list_for_each_entry_safe(adv_instance, n, &hdev->adv_instances,
					 list) {
			if (!(force || adv_instance->timeout))
				continue;

			rem_inst = adv_instance->instance;
			err = hci_remove_adv_instance(hdev, rem_inst);
			if (!err)
				mgmt_advertising_removed(sk, hdev, rem_inst);
		}
	} else {
		adv_instance = hci_find_adv_instance(hdev, instance);

		if (force || (adv_instance && adv_instance->timeout &&
			      !adv_instance->remaining_time)) {
			/* Don't advertise a removed instance. */
			if (next_instance &&
			    next_instance->instance == instance)
				next_instance = NULL;

			err = hci_remove_adv_instance(hdev, instance);
			if (!err)
				mgmt_advertising_removed(sk, hdev, instance);
		}
	}

	if (!req || !hdev_is_powered(hdev) ||
	    hci_dev_test_flag(hdev, HCI_ADVERTISING))
		return;

	if (next_instance && !ext_adv_capable(hdev))
		__hci_req_schedule_adv_instance(req, next_instance->instance,
						false);
}