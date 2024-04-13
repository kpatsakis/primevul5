static int nfs_open(struct device_d *dev, FILE *file, const char *filename)
{
	struct inode *inode = file->f_inode;
	struct nfs_inode *ninode = nfsi(inode);
	struct nfs_priv *npriv = ninode->npriv;
	struct file_priv *priv;

	priv = xzalloc(sizeof(*priv));
	priv->fh = ninode->fh;
	priv->npriv = npriv;
	file->priv = priv;
	file->size = inode->i_size;

	priv->fifo = kfifo_alloc(1024);
	if (!priv->fifo) {
		free(priv);
		return -ENOMEM;
	}

	return 0;
}
