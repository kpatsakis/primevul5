static u8 tdme_chipinit(void *device_ref)
{
	u8 status = MAC_SUCCESS;
	u8 sfr_address;
	struct spi_device *spi = device_ref;
	struct preamble_cfg_sfr pre_cfg_value = {
		.timeout_symbols     = 3,
		.acquisition_symbols = 3,
		.search_symbols      = 1,
	};
	/* LNA Gain Settings */
	status = tdme_setsfr_request_sync(
		1, (sfr_address = CA8210_SFR_LNAGX40),
		LNAGX40_DEFAULT_GAIN, device_ref);
	if (status)
		goto finish;
	status = tdme_setsfr_request_sync(
		1, (sfr_address = CA8210_SFR_LNAGX41),
		LNAGX41_DEFAULT_GAIN, device_ref);
	if (status)
		goto finish;
	status = tdme_setsfr_request_sync(
		1, (sfr_address = CA8210_SFR_LNAGX42),
		LNAGX42_DEFAULT_GAIN, device_ref);
	if (status)
		goto finish;
	status = tdme_setsfr_request_sync(
		1, (sfr_address = CA8210_SFR_LNAGX43),
		LNAGX43_DEFAULT_GAIN, device_ref);
	if (status)
		goto finish;
	status = tdme_setsfr_request_sync(
		1, (sfr_address = CA8210_SFR_LNAGX44),
		LNAGX44_DEFAULT_GAIN, device_ref);
	if (status)
		goto finish;
	status = tdme_setsfr_request_sync(
		1, (sfr_address = CA8210_SFR_LNAGX45),
		LNAGX45_DEFAULT_GAIN, device_ref);
	if (status)
		goto finish;
	status = tdme_setsfr_request_sync(
		1, (sfr_address = CA8210_SFR_LNAGX46),
		LNAGX46_DEFAULT_GAIN, device_ref);
	if (status)
		goto finish;
	status = tdme_setsfr_request_sync(
		1, (sfr_address = CA8210_SFR_LNAGX47),
		LNAGX47_DEFAULT_GAIN, device_ref);
	if (status)
		goto finish;
	/* Preamble Timing Config */
	status = tdme_setsfr_request_sync(
		1, (sfr_address = CA8210_SFR_PRECFG),
		*((u8 *)&pre_cfg_value), device_ref);
	if (status)
		goto finish;
	/* Preamble Threshold High */
	status = tdme_setsfr_request_sync(
		1, (sfr_address = CA8210_SFR_PTHRH),
		PTHRH_DEFAULT_THRESHOLD, device_ref);
	if (status)
		goto finish;
	/* Tx Output Power 8 dBm */
	status = tdme_setsfr_request_sync(
		0, (sfr_address = CA8210_SFR_PACFGIB),
		PACFGIB_DEFAULT_CURRENT, device_ref);
	if (status)
		goto finish;

finish:
	if (status != MAC_SUCCESS) {
		dev_err(
			&spi->dev,
			"failed to set sfr at %#03x, status = %#03x\n",
			sfr_address,
			status
		);
	}
	return status;
}