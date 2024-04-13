static void iwl_fw_prph_handler(struct iwl_fw_runtime *fwrt, void *ptr,
				void (*handler)(struct iwl_fw_runtime *,
						const struct iwl_prph_range *,
						u32, void *))
{
	u32 range_len;

	if (fwrt->trans->trans_cfg->device_family >= IWL_DEVICE_FAMILY_AX210) {
		range_len = ARRAY_SIZE(iwl_prph_dump_addr_ax210);
		handler(fwrt, iwl_prph_dump_addr_ax210, range_len, ptr);
	} else if (fwrt->trans->trans_cfg->device_family >=
		   IWL_DEVICE_FAMILY_22000) {
		range_len = ARRAY_SIZE(iwl_prph_dump_addr_22000);
		handler(fwrt, iwl_prph_dump_addr_22000, range_len, ptr);
	} else {
		range_len = ARRAY_SIZE(iwl_prph_dump_addr_comm);
		handler(fwrt, iwl_prph_dump_addr_comm, range_len, ptr);

		if (fwrt->trans->trans_cfg->mq_rx_supported) {
			range_len = ARRAY_SIZE(iwl_prph_dump_addr_9000);
			handler(fwrt, iwl_prph_dump_addr_9000, range_len, ptr);
		}
	}
}