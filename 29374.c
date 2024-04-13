static void nfs_umount_req(struct nfs_priv *npriv)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;
	int pathlen;
	struct packet *nfs_packet;

	pathlen = strlen(npriv->path);

	p = &(data[0]);
	p = rpc_add_credentials(p);

	p = nfs_add_filename(p, pathlen, npriv->path);

	len = p - &(data[0]);

	nfs_packet = rpc_req(npriv, PROG_MOUNT, MOUNT_UMOUNT, data, len);

	if (!IS_ERR(nfs_packet))
		free(nfs_packet);
}
