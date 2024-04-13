static u32 iwl_dump_ini_rxf_ranges(struct iwl_fw_runtime *fwrt,
				   struct iwl_fw_ini_region_cfg *reg)
{
	/* Each Rx fifo needs a different offset and therefore, it's
	 * region can contain only one fifo, i.e. 1 memory range.
	 */
	return 1;
}