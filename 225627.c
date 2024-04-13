static int iwl_fw_rxf_len(struct iwl_fw_runtime *fwrt,
			  struct iwl_fwrt_shared_mem_cfg *mem_cfg)
{
	size_t hdr_len = sizeof(struct iwl_fw_error_dump_data) +
			 sizeof(struct iwl_fw_error_dump_fifo);
	u32 fifo_len = 0;
	int i;

	if (!iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_RXF))
		return 0;

	/* Count RXF2 size */
	ADD_LEN(fifo_len, mem_cfg->rxfifo2_size, hdr_len);

	/* Count RXF1 sizes */
	if (WARN_ON(mem_cfg->num_lmacs > MAX_NUM_LMAC))
		mem_cfg->num_lmacs = MAX_NUM_LMAC;

	for (i = 0; i < mem_cfg->num_lmacs; i++)
		ADD_LEN(fifo_len, mem_cfg->lmac[i].rxfifo1_size, hdr_len);

	return fifo_len;
}