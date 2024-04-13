int iwl_fw_dbg_collect_desc(struct iwl_fw_runtime *fwrt,
			    const struct iwl_fw_dump_desc *desc,
			    bool monitor_only,
			    unsigned int delay)
{
	u32 trig_type = le32_to_cpu(desc->trig_desc.type);
	int ret;

	if (iwl_trans_dbg_ini_valid(fwrt->trans)) {
		ret = iwl_fw_dbg_ini_collect(fwrt, trig_type);
		if (!ret)
			iwl_fw_free_dump_desc(fwrt);

		return ret;
	}

	/* use wks[0] since dump flow prior to ini does not need to support
	 * consecutive triggers collection
	 */
	if (test_and_set_bit(fwrt->dump.wks[0].idx, &fwrt->dump.active_wks))
		return -EBUSY;

	if (WARN_ON(fwrt->dump.desc))
		iwl_fw_free_dump_desc(fwrt);

	IWL_WARN(fwrt, "Collecting data: trigger %d fired.\n",
		 le32_to_cpu(desc->trig_desc.type));

	fwrt->dump.desc = desc;
	fwrt->dump.monitor_only = monitor_only;

	schedule_delayed_work(&fwrt->dump.wks[0].wk, usecs_to_jiffies(delay));

	return 0;
}