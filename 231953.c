void ext4_fname_setup_ci_filename(struct inode *dir, const struct qstr *iname,
				  struct fscrypt_str *cf_name)
{
	int len;

	if (!IS_CASEFOLDED(dir) || !EXT4_SB(dir->i_sb)->s_encoding) {
		cf_name->name = NULL;
		return;
	}

	cf_name->name = kmalloc(EXT4_NAME_LEN, GFP_NOFS);
	if (!cf_name->name)
		return;

	len = utf8_casefold(EXT4_SB(dir->i_sb)->s_encoding,
			    iname, cf_name->name,
			    EXT4_NAME_LEN);
	if (len <= 0) {
		kfree(cf_name->name);
		cf_name->name = NULL;
		return;
	}
	cf_name->len = (unsigned) len;

}