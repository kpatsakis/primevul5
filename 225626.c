static int iwl_fw_dbg_suspend_resume_hcmd(struct iwl_trans *trans, bool suspend)
{
	struct iwl_dbg_suspend_resume_cmd cmd = {
		.operation = suspend ?
			cpu_to_le32(DBGC_SUSPEND_CMD) :
			cpu_to_le32(DBGC_RESUME_CMD),
	};
	struct iwl_host_cmd hcmd = {
		.id = WIDE_ID(DEBUG_GROUP, DBGC_SUSPEND_RESUME),
		.data[0] = &cmd,
		.len[0] = sizeof(cmd),
	};

	return iwl_trans_send_cmd(trans, &hcmd);
}