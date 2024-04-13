static int nfs_close(struct device_d *dev, FILE *file)
{
	struct file_priv *priv = file->priv;

	nfs_do_close(priv);

	return 0;
}
