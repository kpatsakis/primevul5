iwl_fw_error_dump_file(struct iwl_fw_runtime *fwrt,
		       struct iwl_fw_dump_ptrs *fw_error_dump)
{
	struct iwl_fw_error_dump_file *dump_file;
	struct iwl_fw_error_dump_data *dump_data;
	struct iwl_fw_error_dump_info *dump_info;
	struct iwl_fw_error_dump_smem_cfg *dump_smem_cfg;
	struct iwl_fw_error_dump_trigger_desc *dump_trig;
	u32 sram_len, sram_ofs;
	const struct iwl_fw_dbg_mem_seg_tlv *fw_mem = fwrt->fw->dbg.mem_tlv;
	struct iwl_fwrt_shared_mem_cfg *mem_cfg = &fwrt->smem_cfg;
	u32 file_len, fifo_len = 0, prph_len = 0, radio_len = 0;
	u32 smem_len = fwrt->fw->dbg.n_mem_tlv ? 0 : fwrt->trans->cfg->smem_len;
	u32 sram2_len = fwrt->fw->dbg.n_mem_tlv ?
				0 : fwrt->trans->cfg->dccm2_len;
	int i;

	/* SRAM - include stack CCM if driver knows the values for it */
	if (!fwrt->trans->cfg->dccm_offset || !fwrt->trans->cfg->dccm_len) {
		const struct fw_img *img;

		if (fwrt->cur_fw_img >= IWL_UCODE_TYPE_MAX)
			return NULL;
		img = &fwrt->fw->img[fwrt->cur_fw_img];
		sram_ofs = img->sec[IWL_UCODE_SECTION_DATA].offset;
		sram_len = img->sec[IWL_UCODE_SECTION_DATA].len;
	} else {
		sram_ofs = fwrt->trans->cfg->dccm_offset;
		sram_len = fwrt->trans->cfg->dccm_len;
	}

	/* reading RXF/TXF sizes */
	if (test_bit(STATUS_FW_ERROR, &fwrt->trans->status)) {
		fifo_len = iwl_fw_rxf_len(fwrt, mem_cfg);
		fifo_len += iwl_fw_txf_len(fwrt, mem_cfg);

		/* Make room for PRPH registers */
		if (iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_PRPH))
			iwl_fw_prph_handler(fwrt, &prph_len,
					    iwl_fw_get_prph_len);

		if (fwrt->trans->trans_cfg->device_family ==
		    IWL_DEVICE_FAMILY_7000 &&
		    iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_RADIO_REG))
			radio_len = sizeof(*dump_data) + RADIO_REG_MAX_READ;
	}

	file_len = sizeof(*dump_file) + fifo_len + prph_len + radio_len;

	if (iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_DEV_FW_INFO))
		file_len += sizeof(*dump_data) + sizeof(*dump_info);
	if (iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_MEM_CFG))
		file_len += sizeof(*dump_data) + sizeof(*dump_smem_cfg);

	if (iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_MEM)) {
		size_t hdr_len = sizeof(*dump_data) +
				 sizeof(struct iwl_fw_error_dump_mem);

		/* Dump SRAM only if no mem_tlvs */
		if (!fwrt->fw->dbg.n_mem_tlv)
			ADD_LEN(file_len, sram_len, hdr_len);

		/* Make room for all mem types that exist */
		ADD_LEN(file_len, smem_len, hdr_len);
		ADD_LEN(file_len, sram2_len, hdr_len);

		for (i = 0; i < fwrt->fw->dbg.n_mem_tlv; i++)
			ADD_LEN(file_len, le32_to_cpu(fw_mem[i].len), hdr_len);
	}

	/* Make room for fw's virtual image pages, if it exists */
	if (iwl_fw_dbg_is_paging_enabled(fwrt))
		file_len += fwrt->num_of_paging_blk *
			(sizeof(*dump_data) +
			 sizeof(struct iwl_fw_error_dump_paging) +
			 PAGING_BLOCK_SIZE);

	if (iwl_fw_dbg_is_d3_debug_enabled(fwrt) && fwrt->dump.d3_debug_data) {
		file_len += sizeof(*dump_data) +
			fwrt->trans->cfg->d3_debug_data_length * 2;
	}

	/* If we only want a monitor dump, reset the file length */
	if (fwrt->dump.monitor_only) {
		file_len = sizeof(*dump_file) + sizeof(*dump_data) * 2 +
			   sizeof(*dump_info) + sizeof(*dump_smem_cfg);
	}

	if (iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_ERROR_INFO) &&
	    fwrt->dump.desc)
		file_len += sizeof(*dump_data) + sizeof(*dump_trig) +
			    fwrt->dump.desc->len;

	dump_file = vzalloc(file_len);
	if (!dump_file)
		return NULL;

	fw_error_dump->fwrt_ptr = dump_file;

	dump_file->barker = cpu_to_le32(IWL_FW_ERROR_DUMP_BARKER);
	dump_data = (void *)dump_file->data;

	if (iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_DEV_FW_INFO)) {
		dump_data->type = cpu_to_le32(IWL_FW_ERROR_DUMP_DEV_FW_INFO);
		dump_data->len = cpu_to_le32(sizeof(*dump_info));
		dump_info = (void *)dump_data->data;
		dump_info->hw_type =
			cpu_to_le32(CSR_HW_REV_TYPE(fwrt->trans->hw_rev));
		dump_info->hw_step =
			cpu_to_le32(CSR_HW_REV_STEP(fwrt->trans->hw_rev));
		memcpy(dump_info->fw_human_readable, fwrt->fw->human_readable,
		       sizeof(dump_info->fw_human_readable));
		strncpy(dump_info->dev_human_readable, fwrt->trans->cfg->name,
			sizeof(dump_info->dev_human_readable) - 1);
		strncpy(dump_info->bus_human_readable, fwrt->dev->bus->name,
			sizeof(dump_info->bus_human_readable) - 1);
		dump_info->num_of_lmacs = fwrt->smem_cfg.num_lmacs;
		dump_info->lmac_err_id[0] =
			cpu_to_le32(fwrt->dump.lmac_err_id[0]);
		if (fwrt->smem_cfg.num_lmacs > 1)
			dump_info->lmac_err_id[1] =
				cpu_to_le32(fwrt->dump.lmac_err_id[1]);
		dump_info->umac_err_id = cpu_to_le32(fwrt->dump.umac_err_id);

		dump_data = iwl_fw_error_next_data(dump_data);
	}

	if (iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_MEM_CFG)) {
		/* Dump shared memory configuration */
		dump_data->type = cpu_to_le32(IWL_FW_ERROR_DUMP_MEM_CFG);
		dump_data->len = cpu_to_le32(sizeof(*dump_smem_cfg));
		dump_smem_cfg = (void *)dump_data->data;
		dump_smem_cfg->num_lmacs = cpu_to_le32(mem_cfg->num_lmacs);
		dump_smem_cfg->num_txfifo_entries =
			cpu_to_le32(mem_cfg->num_txfifo_entries);
		for (i = 0; i < MAX_NUM_LMAC; i++) {
			int j;
			u32 *txf_size = mem_cfg->lmac[i].txfifo_size;

			for (j = 0; j < TX_FIFO_MAX_NUM; j++)
				dump_smem_cfg->lmac[i].txfifo_size[j] =
					cpu_to_le32(txf_size[j]);
			dump_smem_cfg->lmac[i].rxfifo1_size =
				cpu_to_le32(mem_cfg->lmac[i].rxfifo1_size);
		}
		dump_smem_cfg->rxfifo2_size =
			cpu_to_le32(mem_cfg->rxfifo2_size);
		dump_smem_cfg->internal_txfifo_addr =
			cpu_to_le32(mem_cfg->internal_txfifo_addr);
		for (i = 0; i < TX_FIFO_INTERNAL_MAX_NUM; i++) {
			dump_smem_cfg->internal_txfifo_size[i] =
				cpu_to_le32(mem_cfg->internal_txfifo_size[i]);
		}

		dump_data = iwl_fw_error_next_data(dump_data);
	}

	/* We only dump the FIFOs if the FW is in error state */
	if (fifo_len) {
		iwl_fw_dump_rxf(fwrt, &dump_data);
		iwl_fw_dump_txf(fwrt, &dump_data);
	}

	if (radio_len)
		iwl_read_radio_regs(fwrt, &dump_data);

	if (iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_ERROR_INFO) &&
	    fwrt->dump.desc) {
		dump_data->type = cpu_to_le32(IWL_FW_ERROR_DUMP_ERROR_INFO);
		dump_data->len = cpu_to_le32(sizeof(*dump_trig) +
					     fwrt->dump.desc->len);
		dump_trig = (void *)dump_data->data;
		memcpy(dump_trig, &fwrt->dump.desc->trig_desc,
		       sizeof(*dump_trig) + fwrt->dump.desc->len);

		dump_data = iwl_fw_error_next_data(dump_data);
	}

	/* In case we only want monitor dump, skip to dump trasport data */
	if (fwrt->dump.monitor_only)
		goto out;

	if (iwl_fw_dbg_type_on(fwrt, IWL_FW_ERROR_DUMP_MEM)) {
		const struct iwl_fw_dbg_mem_seg_tlv *fw_dbg_mem =
			fwrt->fw->dbg.mem_tlv;

		if (!fwrt->fw->dbg.n_mem_tlv)
			iwl_fw_dump_mem(fwrt, &dump_data, sram_len, sram_ofs,
					IWL_FW_ERROR_DUMP_MEM_SRAM);

		for (i = 0; i < fwrt->fw->dbg.n_mem_tlv; i++) {
			u32 len = le32_to_cpu(fw_dbg_mem[i].len);
			u32 ofs = le32_to_cpu(fw_dbg_mem[i].ofs);

			iwl_fw_dump_mem(fwrt, &dump_data, len, ofs,
					le32_to_cpu(fw_dbg_mem[i].data_type));
		}

		iwl_fw_dump_mem(fwrt, &dump_data, smem_len,
				fwrt->trans->cfg->smem_offset,
				IWL_FW_ERROR_DUMP_MEM_SMEM);

		iwl_fw_dump_mem(fwrt, &dump_data, sram2_len,
				fwrt->trans->cfg->dccm2_offset,
				IWL_FW_ERROR_DUMP_MEM_SRAM);
	}

	if (iwl_fw_dbg_is_d3_debug_enabled(fwrt) && fwrt->dump.d3_debug_data) {
		u32 addr = fwrt->trans->cfg->d3_debug_data_base_addr;
		size_t data_size = fwrt->trans->cfg->d3_debug_data_length;

		dump_data->type = cpu_to_le32(IWL_FW_ERROR_DUMP_D3_DEBUG_DATA);
		dump_data->len = cpu_to_le32(data_size * 2);

		memcpy(dump_data->data, fwrt->dump.d3_debug_data, data_size);

		kfree(fwrt->dump.d3_debug_data);
		fwrt->dump.d3_debug_data = NULL;

		iwl_trans_read_mem_bytes(fwrt->trans, addr,
					 dump_data->data + data_size,
					 data_size);

		dump_data = iwl_fw_error_next_data(dump_data);
	}

	/* Dump fw's virtual image */
	if (iwl_fw_dbg_is_paging_enabled(fwrt))
		iwl_dump_paging(fwrt, &dump_data);

	if (prph_len)
		iwl_fw_prph_handler(fwrt, &dump_data, iwl_dump_prph);

out:
	dump_file->file_len = cpu_to_le32(file_len);
	return dump_file;
}