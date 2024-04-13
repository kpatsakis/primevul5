static int iwl_dump_ini_dev_mem_iter(struct iwl_fw_runtime *fwrt,
				     struct iwl_fw_ini_region_cfg *reg,
				     void *range_ptr, int idx)
{
	struct iwl_fw_ini_error_dump_range *range = range_ptr;
	u32 addr = le32_to_cpu(reg->start_addr[idx]) + le32_to_cpu(reg->offset);

	range->internal_base_addr = cpu_to_le32(addr);
	range->range_data_size = reg->internal.range_data_size;
	iwl_trans_read_mem_bytes(fwrt->trans, addr, range->data,
				 le32_to_cpu(reg->internal.range_data_size));

	return sizeof(*range) + le32_to_cpu(range->range_data_size);
}