int iwl_fw_start_dbg_conf(struct iwl_fw_runtime *fwrt, u8 conf_id)
{
	u8 *ptr;
	int ret;
	int i;

	if (WARN_ONCE(conf_id >= ARRAY_SIZE(fwrt->fw->dbg.conf_tlv),
		      "Invalid configuration %d\n", conf_id))
		return -EINVAL;

	/* EARLY START - firmware's configuration is hard coded */
	if ((!fwrt->fw->dbg.conf_tlv[conf_id] ||
	     !fwrt->fw->dbg.conf_tlv[conf_id]->num_of_hcmds) &&
	    conf_id == FW_DBG_START_FROM_ALIVE)
		return 0;

	if (!fwrt->fw->dbg.conf_tlv[conf_id])
		return -EINVAL;

	if (fwrt->dump.conf != FW_DBG_INVALID)
		IWL_WARN(fwrt, "FW already configured (%d) - re-configuring\n",
			 fwrt->dump.conf);

	/* Send all HCMDs for configuring the FW debug */
	ptr = (void *)&fwrt->fw->dbg.conf_tlv[conf_id]->hcmd;
	for (i = 0; i < fwrt->fw->dbg.conf_tlv[conf_id]->num_of_hcmds; i++) {
		struct iwl_fw_dbg_conf_hcmd *cmd = (void *)ptr;
		struct iwl_host_cmd hcmd = {
			.id = cmd->id,
			.len = { le16_to_cpu(cmd->len), },
			.data = { cmd->data, },
		};

		ret = iwl_trans_send_cmd(fwrt->trans, &hcmd);
		if (ret)
			return ret;

		ptr += sizeof(*cmd);
		ptr += le16_to_cpu(cmd->len);
	}

	fwrt->dump.conf = conf_id;

	return 0;
}