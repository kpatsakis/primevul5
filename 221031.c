static uint16_t nvme_tx(NvmeCtrl *n, NvmeSg *sg, uint8_t *ptr, uint32_t len,
                        NvmeTxDirection dir)
{
    assert(sg->flags & NVME_SG_ALLOC);

    if (sg->flags & NVME_SG_DMA) {
        const MemTxAttrs attrs = MEMTXATTRS_UNSPECIFIED;
        dma_addr_t residual;

        if (dir == NVME_TX_DIRECTION_TO_DEVICE) {
            dma_buf_write(ptr, len, &residual, &sg->qsg, attrs);
        } else {
            dma_buf_read(ptr, len, &residual, &sg->qsg, attrs);
        }

        if (unlikely(residual)) {
            trace_pci_nvme_err_invalid_dma();
            return NVME_INVALID_FIELD | NVME_DNR;
        }
    } else {
        size_t bytes;

        if (dir == NVME_TX_DIRECTION_TO_DEVICE) {
            bytes = qemu_iovec_to_buf(&sg->iov, 0, ptr, len);
        } else {
            bytes = qemu_iovec_from_buf(&sg->iov, 0, ptr, len);
        }

        if (unlikely(bytes != len)) {
            trace_pci_nvme_err_invalid_dma();
            return NVME_INVALID_FIELD | NVME_DNR;
        }
    }

    return NVME_SUCCESS;
}