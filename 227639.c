static int ca8210_test_check_upstream(u8 *buf, void *device_ref)
{
	int ret;
	u8 response[CA8210_SPI_BUF_SIZE];

	if (buf[0] == SPI_MLME_SET_REQUEST) {
		ret = tdme_checkpibattribute(buf[2], buf[4], buf + 5);
		if (ret) {
			response[0]  = SPI_MLME_SET_CONFIRM;
			response[1] = 3;
			response[2] = MAC_INVALID_PARAMETER;
			response[3] = buf[2];
			response[4] = buf[3];
			if (cascoda_api_upstream)
				cascoda_api_upstream(response, 5, device_ref);
			return ret;
		}
	}
	if (buf[0] == SPI_MLME_ASSOCIATE_REQUEST) {
		return tdme_channelinit(buf[2], device_ref);
	} else if (buf[0] == SPI_MLME_START_REQUEST) {
		return tdme_channelinit(buf[4], device_ref);
	} else if (
		(buf[0] == SPI_MLME_SET_REQUEST) &&
		(buf[2] == PHY_CURRENT_CHANNEL)
	) {
		return tdme_channelinit(buf[5], device_ref);
	} else if (
		(buf[0] == SPI_TDME_SET_REQUEST) &&
		(buf[2] == TDME_CHANNEL)
	) {
		return tdme_channelinit(buf[4], device_ref);
	} else if (
		(CA8210_MAC_WORKAROUNDS) &&
		(buf[0] == SPI_MLME_RESET_REQUEST) &&
		(buf[2] == 1)
	) {
		/* reset COORD Bit for Channel Filtering as Coordinator */
		return tdme_setsfr_request_sync(
			0,
			CA8210_SFR_MACCON,
			0,
			device_ref
		);
	}
	return 0;
} /* End of EVBMECheckSerialCommand() */