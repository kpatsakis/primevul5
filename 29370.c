static void nfs_remove(struct device_d *dev)
{
	struct nfs_priv *npriv = dev->priv;

	nfs_umount_req(npriv);

	net_unregister(npriv->con);
	free(npriv->path);
	free(npriv);
}
