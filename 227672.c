static u8 mlme_reset_request_sync(
	u8    set_default_pib,
	void *device_ref
)
{
	u8 status;
	struct mac_message command, response;
	struct spi_device *spi = device_ref;

	command.command_id = SPI_MLME_RESET_REQUEST;
	command.length = 1;
	command.pdata.u8param = set_default_pib;

	if (cascoda_api_downstream(
		&command.command_id,
		command.length + 2,
		&response.command_id,
		device_ref)) {
		dev_err(&spi->dev, "cascoda_api_downstream failed\n");
		return MAC_SYSTEM_ERROR;
	}

	if (response.command_id != SPI_MLME_RESET_CONFIRM)
		return MAC_SYSTEM_ERROR;

	status = response.pdata.status;

	/* reset COORD Bit for Channel Filtering as Coordinator */
	if (CA8210_MAC_WORKAROUNDS && set_default_pib && !status) {
		status = tdme_setsfr_request_sync(
			0,
			CA8210_SFR_MACCON,
			0,
			device_ref
		);
	}

	return status;
}