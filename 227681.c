static u8 hwme_get_request_sync(
	u8           hw_attribute,
	u8          *hw_attribute_length,
	u8          *hw_attribute_value,
	void        *device_ref
)
{
	struct mac_message command, response;

	command.command_id = SPI_HWME_GET_REQUEST;
	command.length = 1;
	command.pdata.hwme_get_req.hw_attribute = hw_attribute;

	if (cascoda_api_downstream(
		&command.command_id,
		command.length + 2,
		&response.command_id,
		device_ref)) {
		return MAC_SYSTEM_ERROR;
	}

	if (response.command_id != SPI_HWME_GET_CONFIRM)
		return MAC_SYSTEM_ERROR;

	if (response.pdata.hwme_get_cnf.status == MAC_SUCCESS) {
		*hw_attribute_length =
			response.pdata.hwme_get_cnf.hw_attribute_length;
		memcpy(
			hw_attribute_value,
			response.pdata.hwme_get_cnf.hw_attribute_value,
			*hw_attribute_length
		);
	}

	return response.pdata.hwme_get_cnf.status;
}