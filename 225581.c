static int _iwl_dump_ini_paging_iter(struct iwl_fw_runtime *fwrt,
				     struct iwl_fw_ini_region_cfg *reg,
				     void *range_ptr, int idx)
{
	/* increase idx by 1 since the pages are from 1 to
	 * fwrt->num_of_paging_blk + 1
	 */
	struct page *page = fwrt->fw_paging_db[++idx].fw_paging_block;
	struct iwl_fw_ini_error_dump_range *range = range_ptr;
	dma_addr_t addr = fwrt->fw_paging_db[idx].fw_paging_phys;
	u32 page_size = fwrt->fw_paging_db[idx].fw_paging_size;

	range->page_num = cpu_to_le32(idx);
	range->range_data_size = cpu_to_le32(page_size);
	dma_sync_single_for_cpu(fwrt->trans->dev, addr,	page_size,
				DMA_BIDIRECTIONAL);
	memcpy(range->data, page_address(page), page_size);
	dma_sync_single_for_device(fwrt->trans->dev, addr, page_size,
				   DMA_BIDIRECTIONAL);

	return sizeof(*range) + le32_to_cpu(range->range_data_size);
}