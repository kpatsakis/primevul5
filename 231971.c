static struct stats dx_show_leaf(struct inode *dir,
				struct dx_hash_info *hinfo,
				struct ext4_dir_entry_2 *de,
				int size, int show_names)
{
	unsigned names = 0, space = 0;
	char *base = (char *) de;
	struct dx_hash_info h = *hinfo;

	printk("names: ");
	while ((char *) de < base + size)
	{
		if (de->inode)
		{
			if (show_names)
			{
#ifdef CONFIG_FS_ENCRYPTION
				int len;
				char *name;
				struct fscrypt_str fname_crypto_str =
					FSTR_INIT(NULL, 0);
				int res = 0;

				name  = de->name;
				len = de->name_len;
				if (IS_ENCRYPTED(dir))
					res = fscrypt_get_encryption_info(dir);
				if (res) {
					printk(KERN_WARNING "Error setting up"
					       " fname crypto: %d\n", res);
				}
				if (!fscrypt_has_encryption_key(dir)) {
					/* Directory is not encrypted */
					ext4fs_dirhash(dir, de->name,
						de->name_len, &h);
					printk("%*.s:(U)%x.%u ", len,
					       name, h.hash,
					       (unsigned) ((char *) de
							   - base));
				} else {
					struct fscrypt_str de_name =
						FSTR_INIT(name, len);

					/* Directory is encrypted */
					res = fscrypt_fname_alloc_buffer(
						dir, len,
						&fname_crypto_str);
					if (res)
						printk(KERN_WARNING "Error "
							"allocating crypto "
							"buffer--skipping "
							"crypto\n");
					res = fscrypt_fname_disk_to_usr(dir,
						0, 0, &de_name,
						&fname_crypto_str);
					if (res) {
						printk(KERN_WARNING "Error "
							"converting filename "
							"from disk to usr"
							"\n");
						name = "??";
						len = 2;
					} else {
						name = fname_crypto_str.name;
						len = fname_crypto_str.len;
					}
					ext4fs_dirhash(dir, de->name,
						       de->name_len, &h);
					printk("%*.s:(E)%x.%u ", len, name,
					       h.hash, (unsigned) ((char *) de
								   - base));
					fscrypt_fname_free_buffer(
							&fname_crypto_str);
				}
#else
				int len = de->name_len;
				char *name = de->name;
				ext4fs_dirhash(dir, de->name, de->name_len, &h);
				printk("%*.s:%x.%u ", len, name, h.hash,
				       (unsigned) ((char *) de - base));
#endif
			}
			space += EXT4_DIR_REC_LEN(de->name_len);
			names++;
		}
		de = ext4_next_entry(de, size);
	}
	printk(KERN_CONT "(%i)\n", names);
	return (struct stats) { names, space, 1 };
}