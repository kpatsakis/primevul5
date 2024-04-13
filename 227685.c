static int ca8210_test_interface_init(struct ca8210_priv *priv)
{
	struct ca8210_test *test = &priv->test;
	char node_name[32];

	snprintf(
		node_name,
		sizeof(node_name),
		"ca8210@%d_%d",
		priv->spi->master->bus_num,
		priv->spi->chip_select
	);

	test->ca8210_dfs_spi_int = debugfs_create_file(
		node_name,
		0600, /* S_IRUSR | S_IWUSR */
		NULL,
		priv,
		&test_int_fops
	);
	if (IS_ERR(test->ca8210_dfs_spi_int)) {
		dev_err(
			&priv->spi->dev,
			"Error %ld when creating debugfs node\n",
			PTR_ERR(test->ca8210_dfs_spi_int)
		);
		return PTR_ERR(test->ca8210_dfs_spi_int);
	}
	debugfs_create_symlink("ca8210", NULL, node_name);
	init_waitqueue_head(&test->readq);
	return kfifo_alloc(
		&test->up_fifo,
		CA8210_TEST_INT_FIFO_SIZE,
		GFP_KERNEL
	);
}