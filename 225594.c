static u32 iwl_dump_ini_info(struct iwl_fw_runtime *fwrt,
			     struct iwl_fw_ini_trigger *trigger,
			     struct list_head *list)
{
	struct iwl_fw_ini_dump_entry *entry;
	struct iwl_fw_error_dump_data *tlv;
	struct iwl_fw_ini_dump_info *dump;
	u32 reg_ids_size = le32_to_cpu(trigger->num_regions) * sizeof(__le32);
	u32 size = sizeof(*tlv) + sizeof(*dump) + reg_ids_size;

	entry = kmalloc(sizeof(*entry) + size, GFP_KERNEL);
	if (!entry)
		return 0;

	entry->size = size;

	tlv = (void *)entry->data;
	tlv->type = cpu_to_le32(IWL_INI_DUMP_INFO_TYPE);
	tlv->len = cpu_to_le32(sizeof(*dump) + reg_ids_size);

	dump = (void *)tlv->data;

	dump->version = cpu_to_le32(IWL_INI_DUMP_VER);
	dump->trigger_id = trigger->trigger_id;
	dump->is_external_cfg =
		cpu_to_le32(fwrt->trans->dbg.external_ini_cfg);

	dump->ver_type = cpu_to_le32(fwrt->dump.fw_ver.type);
	dump->ver_subtype = cpu_to_le32(fwrt->dump.fw_ver.subtype);

	dump->hw_step = cpu_to_le32(CSR_HW_REV_STEP(fwrt->trans->hw_rev));
	dump->hw_type = cpu_to_le32(CSR_HW_REV_TYPE(fwrt->trans->hw_rev));

	dump->rf_id_flavor =
		cpu_to_le32(CSR_HW_RFID_FLAVOR(fwrt->trans->hw_rf_id));
	dump->rf_id_dash = cpu_to_le32(CSR_HW_RFID_DASH(fwrt->trans->hw_rf_id));
	dump->rf_id_step = cpu_to_le32(CSR_HW_RFID_STEP(fwrt->trans->hw_rf_id));
	dump->rf_id_type = cpu_to_le32(CSR_HW_RFID_TYPE(fwrt->trans->hw_rf_id));

	dump->lmac_major = cpu_to_le32(fwrt->dump.fw_ver.lmac_major);
	dump->lmac_minor = cpu_to_le32(fwrt->dump.fw_ver.lmac_minor);
	dump->umac_major = cpu_to_le32(fwrt->dump.fw_ver.umac_major);
	dump->umac_minor = cpu_to_le32(fwrt->dump.fw_ver.umac_minor);

	dump->build_tag_len = cpu_to_le32(sizeof(dump->build_tag));
	memcpy(dump->build_tag, fwrt->fw->human_readable,
	       sizeof(dump->build_tag));

	dump->img_name_len = cpu_to_le32(sizeof(dump->img_name));
	memcpy(dump->img_name, fwrt->dump.img_name, sizeof(dump->img_name));

	dump->internal_dbg_cfg_name_len =
		cpu_to_le32(sizeof(dump->internal_dbg_cfg_name));
	memcpy(dump->internal_dbg_cfg_name, fwrt->dump.internal_dbg_cfg_name,
	       sizeof(dump->internal_dbg_cfg_name));

	dump->external_dbg_cfg_name_len =
		cpu_to_le32(sizeof(dump->external_dbg_cfg_name));

	memcpy(dump->external_dbg_cfg_name, fwrt->dump.external_dbg_cfg_name,
	       sizeof(dump->external_dbg_cfg_name));

	dump->regions_num = trigger->num_regions;
	memcpy(dump->region_ids, trigger->data, reg_ids_size);

	/* add dump info TLV to the beginning of the list since it needs to be
	 * the first TLV in the dump
	 */
	list_add(&entry->list, list);

	return entry->size;
}