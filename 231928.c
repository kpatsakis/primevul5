static inline bool ext4_match(const struct inode *parent,
			      const struct ext4_filename *fname,
			      const struct ext4_dir_entry_2 *de)
{
	struct fscrypt_name f;
#ifdef CONFIG_UNICODE
	const struct qstr entry = {.name = de->name, .len = de->name_len};
#endif

	if (!de->inode)
		return false;

	f.usr_fname = fname->usr_fname;
	f.disk_name = fname->disk_name;
#ifdef CONFIG_FS_ENCRYPTION
	f.crypto_buf = fname->crypto_buf;
#endif

#ifdef CONFIG_UNICODE
	if (EXT4_SB(parent->i_sb)->s_encoding && IS_CASEFOLDED(parent)) {
		if (fname->cf_name.name) {
			struct qstr cf = {.name = fname->cf_name.name,
					  .len = fname->cf_name.len};
			return !ext4_ci_compare(parent, &cf, &entry, true);
		}
		return !ext4_ci_compare(parent, fname->usr_fname, &entry,
					false);
	}
#endif

	return fscrypt_match_name(&f, de->name, de->name_len);
}