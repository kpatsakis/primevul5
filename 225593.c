static void *iwl_dump_ini_mem_fill_header(struct iwl_fw_runtime *fwrt,
					  struct iwl_fw_ini_region_cfg *reg,
					  void *data)
{
	struct iwl_fw_ini_error_dump *dump = data;

	dump->header.version = cpu_to_le32(IWL_INI_DUMP_VER);

	return dump->ranges;
}