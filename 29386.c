static void nfs_read_req(int offset, int readlen)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;

	p = &(data[0]);
	p = rpc_add_credentials(p);

	memcpy (p, filefh, NFS_FHSIZE);
	p += (NFS_FHSIZE / 4);
	*p++ = htonl(offset);
	*p++ = htonl(readlen);
	*p++ = 0;

	len = p - &(data[0]);

	rpc_req(PROG_NFS, NFS_READ, data, len);
}
