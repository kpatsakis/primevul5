iwl_dump_ini_mon_dram_iter(struct iwl_fw_runtime *fwrt,
			   struct iwl_fw_ini_region_cfg *reg, void *range_ptr,
			   int idx)
{
	struct iwl_fw_ini_error_dump_range *range = range_ptr;
	u32 start_addr = iwl_read_umac_prph(fwrt->trans,
					    MON_BUFF_BASE_ADDR_VER2);

	if (start_addr == 0x5a5a5a5a)
		return -EBUSY;

	range->dram_base_addr = cpu_to_le64(start_addr);
	range->range_data_size = cpu_to_le32(fwrt->trans->dbg.fw_mon[idx].size);

	memcpy(range->data, fwrt->trans->dbg.fw_mon[idx].block,
	       fwrt->trans->dbg.fw_mon[idx].size);

	return sizeof(*range) + le32_to_cpu(range->range_data_size);
}