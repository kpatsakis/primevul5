static void ca8210_test_interface_clear(struct ca8210_priv *priv)
{
	struct ca8210_test *test = &priv->test;

	debugfs_remove(test->ca8210_dfs_spi_int);
	kfifo_free(&test->up_fifo);
	dev_info(&priv->spi->dev, "Test interface removed\n");
}