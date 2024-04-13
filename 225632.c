int iwl_fw_dbg_error_collect(struct iwl_fw_runtime *fwrt,
			     enum iwl_fw_dbg_trigger trig_type)
{
	int ret;
	struct iwl_fw_dump_desc *iwl_dump_error_desc;

	if (!test_bit(STATUS_DEVICE_ENABLED, &fwrt->trans->status))
		return -EIO;

	iwl_dump_error_desc = kmalloc(sizeof(*iwl_dump_error_desc), GFP_KERNEL);
	if (!iwl_dump_error_desc)
		return -ENOMEM;

	iwl_dump_error_desc->trig_desc.type = cpu_to_le32(trig_type);
	iwl_dump_error_desc->len = 0;

	ret = iwl_fw_dbg_collect_desc(fwrt, iwl_dump_error_desc, false, 0);
	if (ret)
		kfree(iwl_dump_error_desc);
	else
		iwl_trans_sync_nmi(fwrt->trans);

	return ret;
}