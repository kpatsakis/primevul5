int mnt_context_find_umount_fs(struct libmnt_context *cxt,
			       const char *tgt,
			       struct libmnt_fs **pfs)
{
	if (pfs)
		*pfs = NULL;

	if (!cxt || !tgt || !pfs)
		return -EINVAL;

	DBG(CXT, ul_debugobj(cxt, "umount: lookup FS for '%s'", tgt));

	if (!*tgt)
		return 1; /* empty string is not an error */

	/* In future this function should be extended to support for example
	 * fsinfo() (or another cheap way kernel will support), for now the
	 * default is expensive mountinfo/mtab.
	 */
	return __mtab_find_umount_fs(cxt, tgt, pfs);
}