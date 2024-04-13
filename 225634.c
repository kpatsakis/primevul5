*iwl_dump_ini_mon_smem_fill_header(struct iwl_fw_runtime *fwrt,
				   struct iwl_fw_ini_region_cfg *reg,
				   void *data)
{
	struct iwl_fw_ini_monitor_dump *mon_dump = (void *)data;
	const struct iwl_cfg *cfg = fwrt->trans->cfg;

	if (fwrt->trans->trans_cfg->device_family != IWL_DEVICE_FAMILY_9000 &&
	    fwrt->trans->trans_cfg->device_family != IWL_DEVICE_FAMILY_22000) {
		IWL_ERR(fwrt, "Unsupported device family %d\n",
			fwrt->trans->trans_cfg->device_family);
		return NULL;
	}

	return iwl_dump_ini_mon_fill_header(fwrt, reg, mon_dump,
					    cfg->fw_mon_smem_write_ptr_addr,
					    cfg->fw_mon_smem_write_ptr_msk,
					    cfg->fw_mon_smem_cycle_cnt_ptr_addr,
					    cfg->fw_mon_smem_cycle_cnt_ptr_msk);

}