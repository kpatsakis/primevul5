static u8 mcps_data_request(
	u8               src_addr_mode,
	u8               dst_address_mode,
	u16              dst_pan_id,
	union macaddr   *dst_addr,
	u8               msdu_length,
	u8              *msdu,
	u8               msdu_handle,
	u8               tx_options,
	struct secspec  *security,
	void            *device_ref
)
{
	struct secspec *psec;
	struct mac_message command;

	command.command_id = SPI_MCPS_DATA_REQUEST;
	command.pdata.data_req.src_addr_mode = src_addr_mode;
	command.pdata.data_req.dst.mode = dst_address_mode;
	if (dst_address_mode != MAC_MODE_NO_ADDR) {
		command.pdata.data_req.dst.pan_id[0] = LS_BYTE(dst_pan_id);
		command.pdata.data_req.dst.pan_id[1] = MS_BYTE(dst_pan_id);
		if (dst_address_mode == MAC_MODE_SHORT_ADDR) {
			command.pdata.data_req.dst.address[0] = LS_BYTE(
				dst_addr->short_address
			);
			command.pdata.data_req.dst.address[1] = MS_BYTE(
				dst_addr->short_address
			);
		} else {   /* MAC_MODE_LONG_ADDR*/
			memcpy(
				command.pdata.data_req.dst.address,
				dst_addr->ieee_address,
				8
			);
		}
	}
	command.pdata.data_req.msdu_length = msdu_length;
	command.pdata.data_req.msdu_handle = msdu_handle;
	command.pdata.data_req.tx_options = tx_options;
	memcpy(command.pdata.data_req.msdu, msdu, msdu_length);
	psec = (struct secspec *)(command.pdata.data_req.msdu + msdu_length);
	command.length = sizeof(struct mcps_data_request_pset) -
		MAX_DATA_SIZE + msdu_length;
	if (!security || security->security_level == 0) {
		psec->security_level = 0;
		command.length += 1;
	} else {
		*psec = *security;
		command.length += sizeof(struct secspec);
	}

	if (ca8210_spi_transfer(device_ref, &command.command_id,
				command.length + 2))
		return MAC_SYSTEM_ERROR;

	return MAC_SUCCESS;
}