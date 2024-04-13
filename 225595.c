static u32 iwl_dump_ini_mem_ranges(struct iwl_fw_runtime *fwrt,
				   struct iwl_fw_ini_region_cfg *reg)
{
	return le32_to_cpu(reg->internal.num_of_ranges);
}