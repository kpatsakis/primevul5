static int ca8210_test_int_driver_write(
	const u8       *buf,
	size_t          len,
	void           *spi
)
{
	struct ca8210_priv *priv = spi_get_drvdata(spi);
	struct ca8210_test *test = &priv->test;
	char *fifo_buffer;
	int i;

	dev_dbg(
		&priv->spi->dev,
		"test_interface: Buffering upstream message:\n"
	);
	for (i = 0; i < len; i++)
		dev_dbg(&priv->spi->dev, "%#03x\n", buf[i]);

	fifo_buffer = kmemdup(buf, len, GFP_KERNEL);
	if (!fifo_buffer)
		return -ENOMEM;
	kfifo_in(&test->up_fifo, &fifo_buffer, 4);
	wake_up_interruptible(&priv->test.readq);

	return 0;
}