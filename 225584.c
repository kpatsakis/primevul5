static void iwl_fw_dbg_collect_sync(struct iwl_fw_runtime *fwrt, u8 wk_idx)
{
	struct iwl_fw_dbg_params params = {0};

	if (!test_bit(wk_idx, &fwrt->dump.active_wks))
		return;

	if (fwrt->ops && fwrt->ops->fw_running &&
	    !fwrt->ops->fw_running(fwrt->ops_ctx)) {
		IWL_ERR(fwrt, "Firmware not running - cannot dump error\n");
		iwl_fw_free_dump_desc(fwrt);
		goto out;
	}

	/* there's no point in fw dump if the bus is dead */
	if (test_bit(STATUS_TRANS_DEAD, &fwrt->trans->status)) {
		IWL_ERR(fwrt, "Skip fw error dump since bus is dead\n");
		goto out;
	}

	if (iwl_fw_dbg_stop_restart_recording(fwrt, &params, true)) {
		IWL_ERR(fwrt, "Failed to stop DBGC recording, aborting dump\n");
		goto out;
	}

	IWL_DEBUG_FW_INFO(fwrt, "WRT: Data collection start\n");
	if (iwl_trans_dbg_ini_valid(fwrt->trans))
		iwl_fw_error_ini_dump(fwrt, wk_idx);
	else
		iwl_fw_error_dump(fwrt);
	IWL_DEBUG_FW_INFO(fwrt, "WRT: Data collection done\n");

	iwl_fw_dbg_stop_restart_recording(fwrt, &params, false);

out:
	clear_bit(wk_idx, &fwrt->dump.active_wks);
}