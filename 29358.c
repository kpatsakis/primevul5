static int nfs_fattr3_to_stat(uint32_t *p, struct inode *inode)
{
	uint32_t mode;
	size_t i;

	if (!inode)
		return 0;

	/* offsetof(struct fattr3, type) = 0 */
	switch (ntoh32(net_read_uint32(p + 0))) {
	case NF3REG:
		inode->i_mode = S_IFREG;
		break;
	case NF3DIR:
		inode->i_mode = S_IFDIR;
		break;
	case NF3BLK:
		inode->i_mode = S_IFBLK;
		break;
	case NF3CHR:
		inode->i_mode = S_IFCHR;
		break;
	case NF3LNK:
		inode->i_mode = S_IFLNK;
		break;
	case NF3SOCK:
		inode->i_mode = S_IFSOCK;
		break;
	case NF3FIFO:
		inode->i_mode = S_IFIFO;
		break;
	default:
		printf("%s: invalid mode %x\n",
				__func__, ntoh32(net_read_uint32(p + 0)));
		return -EIO;
	}

	/* offsetof(struct fattr3, mode) = 4 */
	mode = ntoh32(net_read_uint32(p + 1));
	for (i = 0; i < ARRAY_SIZE(nfs3_mode_bits); ++i) {
		if (mode & nfs3_mode_bits[i].nfsmode)
			inode->i_mode |= nfs3_mode_bits[i].statmode;
	}

	/* offsetof(struct fattr3, size) = 20 */
	inode->i_size = ntoh64(net_read_uint64(p + 5));

	return 0;
}
