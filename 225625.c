static u32 iwl_dump_ini_mon_smem_get_size(struct iwl_fw_runtime *fwrt,
					  struct iwl_fw_ini_region_cfg *reg)
{
	return sizeof(struct iwl_fw_ini_monitor_dump) +
		iwl_dump_ini_mem_ranges(fwrt, reg) *
		(sizeof(struct iwl_fw_ini_error_dump_range) +
		 le32_to_cpu(reg->internal.range_data_size));
}