static int ca8210_spi_transfer(
	struct spi_device  *spi,
	const u8           *buf,
	size_t              len
)
{
	int i, status = 0;
	struct ca8210_priv *priv;
	struct cas_control *cas_ctl;

	if (!spi) {
		pr_crit("NULL spi device passed to %s\n", __func__);
		return -ENODEV;
	}

	priv = spi_get_drvdata(spi);
	reinit_completion(&priv->spi_transfer_complete);

	dev_dbg(&spi->dev, "%s called\n", __func__);

	cas_ctl = kmalloc(sizeof(*cas_ctl), GFP_ATOMIC);
	if (!cas_ctl)
		return -ENOMEM;

	cas_ctl->priv = priv;
	memset(cas_ctl->tx_buf, SPI_IDLE, CA8210_SPI_BUF_SIZE);
	memset(cas_ctl->tx_in_buf, SPI_IDLE, CA8210_SPI_BUF_SIZE);
	memcpy(cas_ctl->tx_buf, buf, len);

	for (i = 0; i < len; i++)
		dev_dbg(&spi->dev, "%#03x\n", cas_ctl->tx_buf[i]);

	spi_message_init(&cas_ctl->msg);

	cas_ctl->transfer.tx_nbits = 1; /* 1 MOSI line */
	cas_ctl->transfer.rx_nbits = 1; /* 1 MISO line */
	cas_ctl->transfer.speed_hz = 0; /* Use device setting */
	cas_ctl->transfer.bits_per_word = 0; /* Use device setting */
	cas_ctl->transfer.tx_buf = cas_ctl->tx_buf;
	cas_ctl->transfer.rx_buf = cas_ctl->tx_in_buf;
	cas_ctl->transfer.delay_usecs = 0;
	cas_ctl->transfer.cs_change = 0;
	cas_ctl->transfer.len = sizeof(struct mac_message);
	cas_ctl->msg.complete = ca8210_spi_transfer_complete;
	cas_ctl->msg.context = cas_ctl;

	spi_message_add_tail(
		&cas_ctl->transfer,
		&cas_ctl->msg
	);

	status = spi_async(spi, &cas_ctl->msg);
	if (status < 0) {
		dev_crit(
			&spi->dev,
			"status %d from spi_sync in write\n",
			status
		);
	}

	return status;
}