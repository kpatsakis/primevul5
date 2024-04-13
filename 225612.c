void iwl_fw_error_dump_wk(struct work_struct *work)
{
	struct iwl_fw_runtime *fwrt;
	typeof(fwrt->dump.wks[0]) *wks;

	wks = container_of(work, typeof(fwrt->dump.wks[0]), wk.work);
	fwrt = container_of(wks, struct iwl_fw_runtime, dump.wks[wks->idx]);

	/* assumes the op mode mutex is locked in dump_start since
	 * iwl_fw_dbg_collect_sync can't run in parallel
	 */
	if (fwrt->ops && fwrt->ops->dump_start &&
	    fwrt->ops->dump_start(fwrt->ops_ctx))
		return;

	iwl_fw_dbg_collect_sync(fwrt, wks->idx);

	if (fwrt->ops && fwrt->ops->dump_end)
		fwrt->ops->dump_end(fwrt->ops_ctx);
}