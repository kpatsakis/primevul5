static bool iwl_ini_txf_iter(struct iwl_fw_runtime *fwrt,
			     struct iwl_fw_ini_region_cfg *reg, int idx)
{
	struct iwl_txf_iter_data *iter = &fwrt->dump.txf_iter_data;
	struct iwl_fwrt_shared_mem_cfg *cfg = &fwrt->smem_cfg;
	int txf_num = cfg->num_txfifo_entries;
	int int_txf_num = ARRAY_SIZE(cfg->internal_txfifo_size);
	u32 lmac_bitmap = le32_to_cpu(reg->fifos.fid1);

	if (!idx) {
		if (le32_to_cpu(reg->offset) &&
		    WARN_ONCE(cfg->num_lmacs == 1,
			      "Invalid lmac offset: 0x%x\n",
			      le32_to_cpu(reg->offset)))
			return false;

		iter->internal_txf = 0;
		iter->fifo_size = 0;
		iter->fifo = -1;
		if (le32_to_cpu(reg->offset))
			iter->lmac = 1;
		else
			iter->lmac = 0;
	}

	if (!iter->internal_txf)
		for (iter->fifo++; iter->fifo < txf_num; iter->fifo++) {
			iter->fifo_size =
				cfg->lmac[iter->lmac].txfifo_size[iter->fifo];
			if (iter->fifo_size && (lmac_bitmap & BIT(iter->fifo)))
				return true;
		}

	iter->internal_txf = 1;

	if (!fw_has_capa(&fwrt->fw->ucode_capa,
			 IWL_UCODE_TLV_CAPA_EXTEND_SHARED_MEM_CFG))
		return false;

	for (iter->fifo++; iter->fifo < int_txf_num + txf_num; iter->fifo++) {
		iter->fifo_size =
			cfg->internal_txfifo_size[iter->fifo - txf_num];
		if (iter->fifo_size && (lmac_bitmap & BIT(iter->fifo)))
			return true;
	}

	return false;
}