static u32 iwl_dump_ini_rxf_get_size(struct iwl_fw_runtime *fwrt,
				     struct iwl_fw_ini_region_cfg *reg)
{
	struct iwl_ini_rxf_data rx_data;
	u32 size = sizeof(struct iwl_fw_ini_error_dump) +
		sizeof(struct iwl_fw_ini_error_dump_range) +
		le32_to_cpu(reg->fifos.num_of_registers) *
		sizeof(struct iwl_fw_ini_error_dump_register);

	if (reg->fifos.header_only)
		return size;

	iwl_ini_get_rxf_data(fwrt, reg, &rx_data);
	size += rx_data.size;

	return size;
}