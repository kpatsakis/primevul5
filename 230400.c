static u8 bredr_oob_data_present(struct hci_conn *conn)
{
	struct hci_dev *hdev = conn->hdev;
	struct oob_data *data;

	data = hci_find_remote_oob_data(hdev, &conn->dst, BDADDR_BREDR);
	if (!data)
		return 0x00;

	if (bredr_sc_enabled(hdev)) {
		/* When Secure Connections is enabled, then just
		 * return the present value stored with the OOB
		 * data. The stored value contains the right present
		 * information. However it can only be trusted when
		 * not in Secure Connection Only mode.
		 */
		if (!hci_dev_test_flag(hdev, HCI_SC_ONLY))
			return data->present;

		/* When Secure Connections Only mode is enabled, then
		 * the P-256 values are required. If they are not
		 * available, then do not declare that OOB data is
		 * present.
		 */
		if (!memcmp(data->rand256, ZERO_KEY, 16) ||
		    !memcmp(data->hash256, ZERO_KEY, 16))
			return 0x00;

		return 0x02;
	}

	/* When Secure Connections is not enabled or actually
	 * not supported by the hardware, then check that if
	 * P-192 data values are present.
	 */
	if (!memcmp(data->rand192, ZERO_KEY, 16) ||
	    !memcmp(data->hash192, ZERO_KEY, 16))
		return 0x00;

	return 0x01;
}