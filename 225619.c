static u32 iwl_dump_ini_txf_get_size(struct iwl_fw_runtime *fwrt,
				     struct iwl_fw_ini_region_cfg *reg)
{
	struct iwl_txf_iter_data *iter = &fwrt->dump.txf_iter_data;
	u32 size = 0;
	u32 fifo_hdr = sizeof(struct iwl_fw_ini_error_dump_range) +
		le32_to_cpu(reg->fifos.num_of_registers) *
		sizeof(struct iwl_fw_ini_error_dump_register);

	while (iwl_ini_txf_iter(fwrt, reg, size)) {
		size += fifo_hdr;
		if (!reg->fifos.header_only)
			size += iter->fifo_size;
	}

	if (size)
		size += sizeof(struct iwl_fw_ini_error_dump);

	return size;
}