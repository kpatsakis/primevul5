void iwl_fw_dbg_stop_sync(struct iwl_fw_runtime *fwrt)
{
	int i;

	iwl_dbg_tlv_del_timers(fwrt->trans);
	for (i = 0; i < IWL_FW_RUNTIME_DUMP_WK_NUM; i++)
		iwl_fw_dbg_collect_sync(fwrt, i);

	iwl_fw_dbg_stop_restart_recording(fwrt, NULL, true);
}