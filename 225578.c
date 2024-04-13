*iwl_dump_ini_mon_dram_fill_header(struct iwl_fw_runtime *fwrt,
				   struct iwl_fw_ini_region_cfg *reg,
				   void *data)
{
	struct iwl_fw_ini_monitor_dump *mon_dump = (void *)data;
	u32 write_ptr_addr, write_ptr_msk, cycle_cnt_addr, cycle_cnt_msk;

	switch (fwrt->trans->trans_cfg->device_family) {
	case IWL_DEVICE_FAMILY_9000:
	case IWL_DEVICE_FAMILY_22000:
		write_ptr_addr = MON_BUFF_WRPTR_VER2;
		write_ptr_msk = -1;
		cycle_cnt_addr = MON_BUFF_CYCLE_CNT_VER2;
		cycle_cnt_msk = -1;
		break;
	default:
		IWL_ERR(fwrt, "Unsupported device family %d\n",
			fwrt->trans->trans_cfg->device_family);
		return NULL;
	}

	return iwl_dump_ini_mon_fill_header(fwrt, reg, mon_dump, write_ptr_addr,
					    write_ptr_msk, cycle_cnt_addr,
					    cycle_cnt_msk);
}