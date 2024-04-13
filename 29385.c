static void nfs_mount_req(char *path)
{
	uint32_t data[1024];
	uint32_t *p;
	int len;
	int pathlen;

	pathlen = strlen (path);

	p = &(data[0]);
	p = rpc_add_credentials(p);

	*p++ = htonl(pathlen);
	if (pathlen & 3)
		*(p + pathlen / 4) = 0;
	memcpy (p, path, pathlen);
	p += (pathlen + 3) / 4;

	len = p - &(data[0]);

	rpc_req(PROG_MOUNT, MOUNT_ADDENTRY, data, len);
}
