static u8 hwme_set_request_sync(
	u8           hw_attribute,
	u8           hw_attribute_length,
	u8          *hw_attribute_value,
	void        *device_ref
)
{
	struct mac_message command, response;

	command.command_id = SPI_HWME_SET_REQUEST;
	command.length = 2 + hw_attribute_length;
	command.pdata.hwme_set_req.hw_attribute = hw_attribute;
	command.pdata.hwme_set_req.hw_attribute_length = hw_attribute_length;
	memcpy(
		command.pdata.hwme_set_req.hw_attribute_value,
		hw_attribute_value,
		hw_attribute_length
	);

	if (cascoda_api_downstream(
		&command.command_id,
		command.length + 2,
		&response.command_id,
		device_ref)) {
		return MAC_SYSTEM_ERROR;
	}

	if (response.command_id != SPI_HWME_SET_CONFIRM)
		return MAC_SYSTEM_ERROR;

	return response.pdata.hwme_set_cnf.status;
}