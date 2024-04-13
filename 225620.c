int _iwl_fw_dbg_ini_collect(struct iwl_fw_runtime *fwrt,
			    enum iwl_fw_ini_trigger_id id)
{
	struct iwl_fw_ini_active_triggers *active;
	u32 occur, delay;
	unsigned long idx;

	if (WARN_ON(!iwl_fw_ini_trigger_on(fwrt, id)))
		return -EINVAL;

	if (!iwl_fw_ini_trigger_on(fwrt, id)) {
		IWL_WARN(fwrt, "WRT: Trigger %d is not active, aborting dump\n",
			 id);
		return -EINVAL;
	}

	active = &fwrt->dump.active_trigs[id];
	delay = le32_to_cpu(active->trig->dump_delay);
	occur = le32_to_cpu(active->trig->occurrences);
	if (!occur)
		return 0;

	active->trig->occurrences = cpu_to_le32(--occur);

	if (le32_to_cpu(active->trig->force_restart)) {
		IWL_WARN(fwrt, "WRT: Force restart: trigger %d fired.\n", id);
		iwl_force_nmi(fwrt->trans);
		return 0;
	}

	/* Check there is an available worker.
	 * ffz return value is undefined if no zero exists,
	 * so check against ~0UL first.
	 */
	if (fwrt->dump.active_wks == ~0UL)
		return -EBUSY;

	idx = ffz(fwrt->dump.active_wks);

	if (idx >= IWL_FW_RUNTIME_DUMP_WK_NUM ||
	    test_and_set_bit(fwrt->dump.wks[idx].idx, &fwrt->dump.active_wks))
		return -EBUSY;

	fwrt->dump.wks[idx].ini_trig_id = id;

	IWL_WARN(fwrt, "WRT: Collecting data: ini trigger %d fired.\n", id);

	schedule_delayed_work(&fwrt->dump.wks[idx].wk, usecs_to_jiffies(delay));

	return 0;
}