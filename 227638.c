static void ca8210_spi_transfer_complete(void *context)
{
	struct cas_control *cas_ctl = context;
	struct ca8210_priv *priv = cas_ctl->priv;
	bool duplex_rx = false;
	int i;
	u8 retry_buffer[CA8210_SPI_BUF_SIZE];

	if (
		cas_ctl->tx_in_buf[0] == SPI_NACK ||
		(cas_ctl->tx_in_buf[0] == SPI_IDLE &&
		cas_ctl->tx_in_buf[1] == SPI_NACK)
	) {
		/* ca8210 is busy */
		dev_info(&priv->spi->dev, "ca8210 was busy during attempted write\n");
		if (cas_ctl->tx_buf[0] == SPI_IDLE) {
			dev_warn(
				&priv->spi->dev,
				"IRQ servicing NACKd, dropping transfer\n"
			);
			kfree(cas_ctl);
			return;
		}
		if (priv->retries > 3) {
			dev_err(&priv->spi->dev, "too many retries!\n");
			kfree(cas_ctl);
			ca8210_remove(priv->spi);
			return;
		}
		memcpy(retry_buffer, cas_ctl->tx_buf, CA8210_SPI_BUF_SIZE);
		kfree(cas_ctl);
		ca8210_spi_transfer(
			priv->spi,
			retry_buffer,
			CA8210_SPI_BUF_SIZE
		);
		priv->retries++;
		dev_info(&priv->spi->dev, "retried spi write\n");
		return;
	} else if (
			cas_ctl->tx_in_buf[0] != SPI_IDLE &&
			cas_ctl->tx_in_buf[0] != SPI_NACK
		) {
		duplex_rx = true;
	}

	if (duplex_rx) {
		dev_dbg(&priv->spi->dev, "READ CMD DURING TX\n");
		for (i = 0; i < cas_ctl->tx_in_buf[1] + 2; i++)
			dev_dbg(
				&priv->spi->dev,
				"%#03x\n",
				cas_ctl->tx_in_buf[i]
			);
		ca8210_rx_done(cas_ctl);
	}
	complete(&priv->spi_transfer_complete);
	kfree(cas_ctl);
	priv->retries = 0;
}