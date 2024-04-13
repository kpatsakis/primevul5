static void iwl_ini_get_rxf_data(struct iwl_fw_runtime *fwrt,
				 struct iwl_fw_ini_region_cfg *reg,
				 struct iwl_ini_rxf_data *data)
{
	u32 fid1 = le32_to_cpu(reg->fifos.fid1);
	u32 fid2 = le32_to_cpu(reg->fifos.fid2);
	u32 fifo_idx;

	if (!data)
		return;

	memset(data, 0, sizeof(*data));

	if (WARN_ON_ONCE((fid1 && fid2) || (!fid1 && !fid2)))
		return;

	fifo_idx = ffs(fid1) - 1;
	if (fid1 && !WARN_ON_ONCE((~BIT(fifo_idx) & fid1) ||
				  fifo_idx >= MAX_NUM_LMAC)) {
		data->size = fwrt->smem_cfg.lmac[fifo_idx].rxfifo1_size;
		data->fifo_num = fifo_idx;
		return;
	}

	fifo_idx = ffs(fid2) - 1;
	if (fid2 && !WARN_ON_ONCE(fifo_idx != 0)) {
		data->size = fwrt->smem_cfg.rxfifo2_size;
		data->offset = RXF_DIFF_FROM_PREV;
		/* use bit 31 to distinguish between umac and lmac rxf while
		 * parsing the dump
		 */
		data->fifo_num = fifo_idx | IWL_RXF_UMAC_BIT;
		return;
	}
}