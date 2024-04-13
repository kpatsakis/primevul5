static void nfs4_write_cached_acl(struct inode *inode, struct page **pages, size_t pgbase, size_t acl_len)
{
	struct nfs4_cached_acl *acl;
	size_t buflen = sizeof(*acl) + acl_len;

	if (buflen <= PAGE_SIZE) {
		acl = kmalloc(buflen, GFP_KERNEL);
		if (acl == NULL)
			goto out;
		acl->cached = 1;
		_copy_from_pages(acl->data, pages, pgbase, acl_len);
	} else {
		acl = kmalloc(sizeof(*acl), GFP_KERNEL);
		if (acl == NULL)
			goto out;
		acl->cached = 0;
	}
	acl->len = acl_len;
out:
	nfs4_set_cached_acl(inode, acl);
}