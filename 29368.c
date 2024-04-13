static int nfs_read_req(struct file_priv *priv, uint64_t offset,
		uint32_t readlen)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;
	struct packet *nfs_packet;
	uint32_t rlen, eof;

	/*
	 * struct READ3args {
	 * 	nfs_fh3 file;
	 * 	offset3 offset;
	 * 	count3 count;
	 * };
	 *
	 * struct READ3resok {
	 * 	post_op_attr file_attributes;
	 * 	count3 count;
	 * 	bool eof;
	 * 	opaque data<>;
	 * };
	 *
	 * struct READ3resfail {
	 * 	post_op_attr file_attributes;
	 * };
	 *
	 * union READ3res switch (nfsstat3 status) {
	 * case NFS3_OK:
	 * 	READ3resok resok;
	 * default:
	 * 	READ3resfail resfail;
	 * };
	 */
	p = &(data[0]);
	p = rpc_add_credentials(p);

	p = nfs_add_fh3(p, &priv->fh);
	p = nfs_add_uint64(p, offset);
	p = nfs_add_uint32(p, readlen);

	len = p - &(data[0]);

	nfs_packet = rpc_req(priv->npriv, PROG_NFS, NFSPROC3_READ, data, len);
	if (IS_ERR(nfs_packet))
		return PTR_ERR(nfs_packet);

	p = (void *)nfs_packet->data + sizeof(struct rpc_reply) + 4;

	p = nfs_read_post_op_attr(p, NULL);

	rlen = ntoh32(net_read_uint32(p));

	/* skip over count */
	p += 1;

	eof = ntoh32(net_read_uint32(p));

	/*
	 * skip over eof and count embedded in the representation of data
	 * assuming it equals rlen above.
	 */
	p += 2;

	if (readlen && !rlen && !eof) {
		free(nfs_packet);
		return -EIO;
	}

	kfifo_put(priv->fifo, (char *)p, rlen);

	free(nfs_packet);

	return 0;
}
