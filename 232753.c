static void dw_writer(struct dw_spi *dws)
{
	u32 max;
	u16 txw = 0;

	spin_lock(&dws->buf_lock);
	max = tx_max(dws);
	while (max--) {
		/* Set the tx word if the transfer's original "tx" is not null */
		if (dws->tx_end - dws->len) {
			if (dws->n_bytes == 1)
				txw = *(u8 *)(dws->tx);
			else
				txw = *(u16 *)(dws->tx);
		}
		dw_write_io_reg(dws, DW_SPI_DR, txw);
		dws->tx += dws->n_bytes;
	}
	spin_unlock(&dws->buf_lock);
}