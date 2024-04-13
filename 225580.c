static u32 iwl_dump_ini_txf_ranges(struct iwl_fw_runtime *fwrt,
				   struct iwl_fw_ini_region_cfg *reg)
{
	u32 num_of_fifos = 0;

	while (iwl_ini_txf_iter(fwrt, reg, num_of_fifos))
		num_of_fifos++;

	return num_of_fifos;
}