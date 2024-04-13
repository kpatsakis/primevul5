static int iwl_dump_ini_csr_iter(struct iwl_fw_runtime *fwrt,
				 struct iwl_fw_ini_region_cfg *reg,
				 void *range_ptr, int idx)
{
	struct iwl_fw_ini_error_dump_range *range = range_ptr;
	__le32 *val = range->data;
	u32 addr = le32_to_cpu(reg->start_addr[idx]) + le32_to_cpu(reg->offset);
	int i;

	range->internal_base_addr = cpu_to_le32(addr);
	range->range_data_size = reg->internal.range_data_size;
	for (i = 0; i < le32_to_cpu(reg->internal.range_data_size); i += 4)
		*val++ = cpu_to_le32(iwl_trans_read32(fwrt->trans, addr + i));

	return sizeof(*range) + le32_to_cpu(range->range_data_size);
}