static u32 iwl_dump_ini_file_gen(struct iwl_fw_runtime *fwrt,
				 enum iwl_fw_ini_trigger_id trig_id,
				 struct list_head *list)
{
	struct iwl_fw_ini_dump_entry *entry;
	struct iwl_fw_ini_dump_file_hdr *hdr;
	struct iwl_fw_ini_trigger *trigger;
	u32 size;

	if (!iwl_fw_ini_trigger_on(fwrt, trig_id))
		return 0;

	trigger = fwrt->dump.active_trigs[trig_id].trig;
	if (!trigger || !le32_to_cpu(trigger->num_regions))
		return 0;

	entry = kmalloc(sizeof(*entry) + sizeof(*hdr), GFP_KERNEL);
	if (!entry)
		return 0;

	entry->size = sizeof(*hdr);

	size = iwl_dump_ini_trigger(fwrt, trigger, list);
	if (!size) {
		kfree(entry);
		return 0;
	}

	hdr = (void *)entry->data;
	hdr->barker = cpu_to_le32(IWL_FW_INI_ERROR_DUMP_BARKER);
	hdr->file_len = cpu_to_le32(size + entry->size);

	list_add(&entry->list, list);

	return le32_to_cpu(hdr->file_len);
}