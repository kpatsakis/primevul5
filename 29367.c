static int nfs_read(struct device_d *dev, FILE *file, void *buf, size_t insize)
{
	struct file_priv *priv = file->priv;

	if (insize > 1024)
		insize = 1024;

	if (insize && !kfifo_len(priv->fifo)) {
		int ret = nfs_read_req(priv, file->pos, insize);
		if (ret)
			return ret;
	}

	return kfifo_get(priv->fifo, buf, insize);
}
