static void iwl_fw_error_ini_dump(struct iwl_fw_runtime *fwrt, u8 wk_idx)
{
	enum iwl_fw_ini_trigger_id trig_id = fwrt->dump.wks[wk_idx].ini_trig_id;
	struct list_head dump_list = LIST_HEAD_INIT(dump_list);
	struct scatterlist *sg_dump_data;
	u32 file_len;

	file_len = iwl_dump_ini_file_gen(fwrt, trig_id, &dump_list);
	if (!file_len)
		goto out;

	sg_dump_data = alloc_sgtable(file_len);
	if (sg_dump_data) {
		struct iwl_fw_ini_dump_entry *entry;
		int sg_entries = sg_nents(sg_dump_data);
		u32 offs = 0;

		list_for_each_entry(entry, &dump_list, list) {
			sg_pcopy_from_buffer(sg_dump_data, sg_entries,
					     entry->data, entry->size, offs);
			offs += entry->size;
		}
		dev_coredumpsg(fwrt->trans->dev, sg_dump_data, file_len,
			       GFP_KERNEL);
	}
	iwl_dump_ini_list_free(&dump_list);

out:
	fwrt->dump.wks[wk_idx].ini_trig_id = IWL_FW_TRIGGER_ID_INVALID;
}