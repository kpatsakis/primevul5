static u8 mlme_set_request_sync(
	u8            pib_attribute,
	u8            pib_attribute_index,
	u8            pib_attribute_length,
	const void   *pib_attribute_value,
	void         *device_ref
)
{
	u8 status;
	struct mac_message command, response;

	/* pre-check the validity of pib_attribute values that are not checked
	 * in MAC
	 */
	if (tdme_checkpibattribute(
		pib_attribute, pib_attribute_length, pib_attribute_value)) {
		return MAC_INVALID_PARAMETER;
	}

	if (pib_attribute == PHY_CURRENT_CHANNEL) {
		status = tdme_channelinit(
			*((u8 *)pib_attribute_value),
			device_ref
		);
		if (status)
			return status;
	}

	if (pib_attribute == PHY_TRANSMIT_POWER) {
		return tdme_settxpower(
			*((u8 *)pib_attribute_value),
			device_ref
		);
	}

	command.command_id = SPI_MLME_SET_REQUEST;
	command.length = sizeof(struct mlme_set_request_pset) -
		MAX_ATTRIBUTE_SIZE + pib_attribute_length;
	command.pdata.set_req.pib_attribute = pib_attribute;
	command.pdata.set_req.pib_attribute_index = pib_attribute_index;
	command.pdata.set_req.pib_attribute_length = pib_attribute_length;
	memcpy(
		command.pdata.set_req.pib_attribute_value,
		pib_attribute_value,
		pib_attribute_length
	);

	if (cascoda_api_downstream(
		&command.command_id,
		command.length + 2,
		&response.command_id,
		device_ref)) {
		return MAC_SYSTEM_ERROR;
	}

	if (response.command_id != SPI_MLME_SET_CONFIRM)
		return MAC_SYSTEM_ERROR;

	return response.pdata.status;
}