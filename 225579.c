static int iwl_fw_dbg_restart_recording(struct iwl_trans *trans,
					struct iwl_fw_dbg_params *params)
{
	if (!params)
		return -EIO;

	if (trans->trans_cfg->device_family == IWL_DEVICE_FAMILY_7000) {
		iwl_clear_bits_prph(trans, MON_BUFF_SAMPLE_CTL, 0x100);
		iwl_clear_bits_prph(trans, MON_BUFF_SAMPLE_CTL, 0x1);
		iwl_set_bits_prph(trans, MON_BUFF_SAMPLE_CTL, 0x1);
	} else {
		iwl_write_umac_prph(trans, DBGC_IN_SAMPLE, params->in_sample);
		iwl_write_umac_prph(trans, DBGC_OUT_CTRL, params->out_ctrl);
	}

	return 0;
}