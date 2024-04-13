static u32 iwl_dump_ini_paging_ranges(struct iwl_fw_runtime *fwrt,
				      struct iwl_fw_ini_region_cfg *reg)
{
	if (fwrt->trans->trans_cfg->gen2)
		return fwrt->trans->init_dram.paging_cnt;

	return fwrt->num_of_paging_blk;
}