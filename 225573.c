static u32 iwl_dump_ini_mon_dram_get_size(struct iwl_fw_runtime *fwrt,
					  struct iwl_fw_ini_region_cfg *reg)
{
	u32 size = sizeof(struct iwl_fw_ini_monitor_dump) +
		sizeof(struct iwl_fw_ini_error_dump_range);

	if (fwrt->trans->dbg.num_blocks)
		size += fwrt->trans->dbg.fw_mon[0].size;

	return size;
}