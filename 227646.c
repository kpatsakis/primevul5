static u8 tdme_setsfr_request_sync(
	u8            sfr_page,
	u8            sfr_address,
	u8            sfr_value,
	void         *device_ref
)
{
	int ret;
	struct mac_message command, response;
	struct spi_device *spi = device_ref;

	command.command_id = SPI_TDME_SETSFR_REQUEST;
	command.length = 3;
	command.pdata.tdme_set_sfr_req.sfr_page    = sfr_page;
	command.pdata.tdme_set_sfr_req.sfr_address = sfr_address;
	command.pdata.tdme_set_sfr_req.sfr_value   = sfr_value;
	response.command_id = SPI_IDLE;
	ret = cascoda_api_downstream(
		&command.command_id,
		command.length + 2,
		&response.command_id,
		device_ref
	);
	if (ret) {
		dev_crit(&spi->dev, "cascoda_api_downstream returned %d", ret);
		return MAC_SYSTEM_ERROR;
	}

	if (response.command_id != SPI_TDME_SETSFR_CONFIRM) {
		dev_crit(
			&spi->dev,
			"sync response to SPI_TDME_SETSFR_REQUEST was not SPI_TDME_SETSFR_CONFIRM, it was %d\n",
			response.command_id
		);
		return MAC_SYSTEM_ERROR;
	}

	return response.pdata.tdme_set_sfr_cnf.status;
}