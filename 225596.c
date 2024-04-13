static u32 iwl_dump_ini_paging_get_size(struct iwl_fw_runtime *fwrt,
					struct iwl_fw_ini_region_cfg *reg)
{
	int i;
	u32 range_header_len = sizeof(struct iwl_fw_ini_error_dump_range);
	u32 size = sizeof(struct iwl_fw_ini_error_dump);

	if (fwrt->trans->trans_cfg->gen2) {
		for (i = 0; i < iwl_dump_ini_paging_ranges(fwrt, reg); i++)
			size += range_header_len +
				fwrt->trans->init_dram.paging[i].size;
	} else {
		for (i = 1; i <= iwl_dump_ini_paging_ranges(fwrt, reg); i++)
			size += range_header_len +
				fwrt->fw_paging_db[i].fw_paging_size;
	}

	return size;
}