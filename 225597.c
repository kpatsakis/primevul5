static void iwl_dump_paging(struct iwl_fw_runtime *fwrt,
			    struct iwl_fw_error_dump_data **data)
{
	int i;

	IWL_DEBUG_INFO(fwrt, "WRT paging dump\n");
	for (i = 1; i < fwrt->num_of_paging_blk + 1; i++) {
		struct iwl_fw_error_dump_paging *paging;
		struct page *pages =
			fwrt->fw_paging_db[i].fw_paging_block;
		dma_addr_t addr = fwrt->fw_paging_db[i].fw_paging_phys;

		(*data)->type = cpu_to_le32(IWL_FW_ERROR_DUMP_PAGING);
		(*data)->len = cpu_to_le32(sizeof(*paging) +
					     PAGING_BLOCK_SIZE);
		paging =  (void *)(*data)->data;
		paging->index = cpu_to_le32(i);
		dma_sync_single_for_cpu(fwrt->trans->dev, addr,
					PAGING_BLOCK_SIZE,
					DMA_BIDIRECTIONAL);
		memcpy(paging->data, page_address(pages),
		       PAGING_BLOCK_SIZE);
		dma_sync_single_for_device(fwrt->trans->dev, addr,
					   PAGING_BLOCK_SIZE,
					   DMA_BIDIRECTIONAL);
		(*data) = iwl_fw_error_next_data(*data);
	}
}