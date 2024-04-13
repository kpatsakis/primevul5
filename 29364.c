static int nfs_mount_req(struct nfs_priv *npriv)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;
	int pathlen;
	struct packet *nfs_packet;

	pathlen = strlen(npriv->path);

	debug("%s: %s\n", __func__, npriv->path);

	p = &(data[0]);
	p = rpc_add_credentials(p);

	*p++ = hton32(pathlen);
	if (pathlen & 3)
		*(p + pathlen / 4) = 0;

	memcpy (p, npriv->path, pathlen);
	p += (pathlen + 3) / 4;

	len = p - &(data[0]);

	nfs_packet = rpc_req(npriv, PROG_MOUNT, MOUNT_ADDENTRY, data, len);
	if (IS_ERR(nfs_packet))
		return PTR_ERR(nfs_packet);

	p = (void *)nfs_packet->data + sizeof(struct rpc_reply) + 4;

	npriv->rootfh.size = ntoh32(net_read_uint32(p++));
	if (npriv->rootfh.size > NFS3_FHSIZE) {
		printf("%s: file handle too big: %lu\n",
		       __func__, (unsigned long)npriv->rootfh.size);
		free(nfs_packet);
		return -EIO;
	}
	memcpy(npriv->rootfh.data, p, npriv->rootfh.size);

	free(nfs_packet);

	return 0;
}
