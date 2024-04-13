int __hci_req_hci_power_on(struct hci_dev *hdev)
{
	/* Register the available SMP channels (BR/EDR and LE) only when
	 * successfully powering on the controller. This late
	 * registration is required so that LE SMP can clearly decide if
	 * the public address or static address is used.
	 */
	smp_register(hdev);

	return __hci_req_sync(hdev, powered_update_hci, 0, HCI_CMD_TIMEOUT,
			      NULL);
}