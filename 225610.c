static int iwl_dump_ini_paging_iter(struct iwl_fw_runtime *fwrt,
				    struct iwl_fw_ini_region_cfg *reg,
				    void *range_ptr, int idx)
{
	struct iwl_fw_ini_error_dump_range *range;
	u32 page_size;

	if (!fwrt->trans->trans_cfg->gen2)
		return _iwl_dump_ini_paging_iter(fwrt, reg, range_ptr, idx);

	range = range_ptr;
	page_size = fwrt->trans->init_dram.paging[idx].size;

	range->page_num = cpu_to_le32(idx);
	range->range_data_size = cpu_to_le32(page_size);
	memcpy(range->data, fwrt->trans->init_dram.paging[idx].block,
	       page_size);

	return sizeof(*range) + le32_to_cpu(range->range_data_size);
}