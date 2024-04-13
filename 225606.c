static void iwl_fwrt_dump_txf(struct iwl_fw_runtime *fwrt,
			      struct iwl_fw_error_dump_data **dump_data,
			      int size, u32 offset, int fifo_num)
{
	struct iwl_fw_error_dump_fifo *fifo_hdr;
	u32 *fifo_data;
	u32 fifo_len;
	int i;

	fifo_hdr = (void *)(*dump_data)->data;
	fifo_data = (void *)fifo_hdr->data;
	fifo_len = size;

	/* No need to try to read the data if the length is 0 */
	if (fifo_len == 0)
		return;

	/* Add a TLV for the FIFO */
	(*dump_data)->type = cpu_to_le32(IWL_FW_ERROR_DUMP_TXF);
	(*dump_data)->len = cpu_to_le32(fifo_len + sizeof(*fifo_hdr));

	fifo_hdr->fifo_num = cpu_to_le32(fifo_num);
	fifo_hdr->available_bytes =
		cpu_to_le32(iwl_trans_read_prph(fwrt->trans,
						TXF_FIFO_ITEM_CNT + offset));
	fifo_hdr->wr_ptr =
		cpu_to_le32(iwl_trans_read_prph(fwrt->trans,
						TXF_WR_PTR + offset));
	fifo_hdr->rd_ptr =
		cpu_to_le32(iwl_trans_read_prph(fwrt->trans,
						TXF_RD_PTR + offset));
	fifo_hdr->fence_ptr =
		cpu_to_le32(iwl_trans_read_prph(fwrt->trans,
						TXF_FENCE_PTR + offset));
	fifo_hdr->fence_mode =
		cpu_to_le32(iwl_trans_read_prph(fwrt->trans,
						TXF_LOCK_FENCE + offset));

	/* Set the TXF_READ_MODIFY_ADDR to TXF_WR_PTR */
	iwl_trans_write_prph(fwrt->trans, TXF_READ_MODIFY_ADDR + offset,
			     TXF_WR_PTR + offset);

	/* Dummy-read to advance the read pointer to the head */
	iwl_trans_read_prph(fwrt->trans, TXF_READ_MODIFY_DATA + offset);

	/* Read FIFO */
	fifo_len /= sizeof(u32); /* Size in DWORDS */
	for (i = 0; i < fifo_len; i++)
		fifo_data[i] = iwl_trans_read_prph(fwrt->trans,
						  TXF_READ_MODIFY_DATA +
						  offset);
	*dump_data = iwl_fw_error_next_data(*dump_data);
}