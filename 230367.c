static u8 ext_evt_type_to_legacy(struct hci_dev *hdev, u16 evt_type)
{
	if (evt_type & LE_EXT_ADV_LEGACY_PDU) {
		switch (evt_type) {
		case LE_LEGACY_ADV_IND:
			return LE_ADV_IND;
		case LE_LEGACY_ADV_DIRECT_IND:
			return LE_ADV_DIRECT_IND;
		case LE_LEGACY_ADV_SCAN_IND:
			return LE_ADV_SCAN_IND;
		case LE_LEGACY_NONCONN_IND:
			return LE_ADV_NONCONN_IND;
		case LE_LEGACY_SCAN_RSP_ADV:
		case LE_LEGACY_SCAN_RSP_ADV_SCAN:
			return LE_ADV_SCAN_RSP;
		}

		goto invalid;
	}

	if (evt_type & LE_EXT_ADV_CONN_IND) {
		if (evt_type & LE_EXT_ADV_DIRECT_IND)
			return LE_ADV_DIRECT_IND;

		return LE_ADV_IND;
	}

	if (evt_type & LE_EXT_ADV_SCAN_RSP)
		return LE_ADV_SCAN_RSP;

	if (evt_type & LE_EXT_ADV_SCAN_IND)
		return LE_ADV_SCAN_IND;

	if (evt_type == LE_EXT_ADV_NON_CONN_IND ||
	    evt_type & LE_EXT_ADV_DIRECT_IND)
		return LE_ADV_NONCONN_IND;

invalid:
	bt_dev_err_ratelimited(hdev, "Unknown advertising packet type: 0x%02x",
			       evt_type);

	return LE_ADV_INVALID;
}