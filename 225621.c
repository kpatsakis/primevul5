int iwl_fw_dbg_stop_restart_recording(struct iwl_fw_runtime *fwrt,
				      struct iwl_fw_dbg_params *params,
				      bool stop)
{
	int ret = 0;

	/* if the FW crashed or not debug monitor cfg was given, there is
	 * no point in changing the recording state
	 */
	if (test_bit(STATUS_FW_ERROR, &fwrt->trans->status) ||
	    (!fwrt->trans->dbg.dest_tlv &&
	     fwrt->trans->dbg.ini_dest == IWL_FW_INI_LOCATION_INVALID))
		return 0;

	if (fw_has_capa(&fwrt->fw->ucode_capa,
			IWL_UCODE_TLV_CAPA_DBG_SUSPEND_RESUME_CMD_SUPP))
		ret = iwl_fw_dbg_suspend_resume_hcmd(fwrt->trans, stop);
	else if (stop)
		iwl_fw_dbg_stop_recording(fwrt->trans, params);
	else
		ret = iwl_fw_dbg_restart_recording(fwrt->trans, params);
#ifdef CONFIG_IWLWIFI_DEBUGFS
	if (!ret) {
		if (stop)
			fwrt->trans->dbg.rec_on = false;
		else
			iwl_fw_set_dbg_rec_on(fwrt);
	}
#endif

	return ret;
}