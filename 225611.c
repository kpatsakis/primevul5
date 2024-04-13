static u32 iwl_dump_ini_mem(struct iwl_fw_runtime *fwrt, struct list_head *list,
			    struct iwl_fw_ini_region_cfg *reg,
			    const struct iwl_dump_ini_mem_ops *ops)
{
	struct iwl_fw_ini_dump_entry *entry;
	struct iwl_fw_error_dump_data *tlv;
	struct iwl_fw_ini_error_dump_header *header;
	u32 num_of_ranges, i, type = le32_to_cpu(reg->region_type), size;
	void *range;

	if (!ops->get_num_of_ranges || !ops->get_size || !ops->fill_mem_hdr ||
	    !ops->fill_range)
		return 0;

	size = ops->get_size(fwrt, reg);
	if (!size)
		return 0;

	entry = kmalloc(sizeof(*entry) + sizeof(*tlv) + size, GFP_KERNEL);
	if (!entry)
		return 0;

	entry->size = sizeof(*tlv) + size;

	tlv = (void *)entry->data;
	tlv->type = cpu_to_le32(type);
	tlv->len = cpu_to_le32(size);

	IWL_DEBUG_FW(fwrt, "WRT: Collecting region: id=%d, type=%d\n",
		     le32_to_cpu(reg->region_id), type);

	num_of_ranges = ops->get_num_of_ranges(fwrt, reg);

	header = (void *)tlv->data;
	header->region_id = reg->region_id;
	header->num_of_ranges = cpu_to_le32(num_of_ranges);
	header->name_len = cpu_to_le32(min_t(int, IWL_FW_INI_MAX_NAME,
					     le32_to_cpu(reg->name_len)));
	memcpy(header->name, reg->name, le32_to_cpu(header->name_len));

	range = ops->fill_mem_hdr(fwrt, reg, header);
	if (!range) {
		IWL_ERR(fwrt,
			"WRT: Failed to fill region header: id=%d, type=%d\n",
			le32_to_cpu(reg->region_id), type);
		goto out_err;
	}

	for (i = 0; i < num_of_ranges; i++) {
		int range_size = ops->fill_range(fwrt, reg, range, i);

		if (range_size < 0) {
			IWL_ERR(fwrt,
				"WRT: Failed to dump region: id=%d, type=%d\n",
				le32_to_cpu(reg->region_id), type);
			goto out_err;
		}
		range = range + range_size;
	}

	list_add_tail(&entry->list, list);

	return entry->size;

out_err:
	kfree(entry);

	return 0;
}