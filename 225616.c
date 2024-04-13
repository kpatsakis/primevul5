static u32 iwl_dump_ini_trigger(struct iwl_fw_runtime *fwrt,
				struct iwl_fw_ini_trigger *trigger,
				struct list_head *list)
{
	int i;
	u32 size = 0;

	for (i = 0; i < le32_to_cpu(trigger->num_regions); i++) {
		u32 reg_id = le32_to_cpu(trigger->data[i]), reg_type;
		struct iwl_fw_ini_region_cfg *reg;

		if (WARN_ON(reg_id >= ARRAY_SIZE(fwrt->dump.active_regs)))
			continue;

		reg = fwrt->dump.active_regs[reg_id];
		if (!reg) {
			IWL_WARN(fwrt,
				 "WRT: Unassigned region id %d, skipping\n",
				 reg_id);
			continue;
		}

		/* currently the driver supports always on domain only */
		if (le32_to_cpu(reg->domain) != IWL_FW_INI_DBG_DOMAIN_ALWAYS_ON)
			continue;

		reg_type = le32_to_cpu(reg->region_type);
		if (reg_type >= ARRAY_SIZE(iwl_dump_ini_region_ops))
			continue;

		size += iwl_dump_ini_mem(fwrt, list, reg,
					 &iwl_dump_ini_region_ops[reg_type]);
	}

	if (size)
		size += iwl_dump_ini_info(fwrt, trigger, list);

	return size;
}