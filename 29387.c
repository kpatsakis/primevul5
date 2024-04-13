static void nfs_readlink_req(void)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;

	p = &(data[0]);
	p = rpc_add_credentials(p);

	memcpy (p, filefh, NFS_FHSIZE);
	p += (NFS_FHSIZE / 4);

	len = p - &(data[0]);

	rpc_req(PROG_NFS, NFS_READLINK, data, len);
}
