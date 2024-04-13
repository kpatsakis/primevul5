static int htree_dirblock_to_tree(struct file *dir_file,
				  struct inode *dir, ext4_lblk_t block,
				  struct dx_hash_info *hinfo,
				  __u32 start_hash, __u32 start_minor_hash)
{
	struct buffer_head *bh;
	struct ext4_dir_entry_2 *de, *top;
	int err = 0, count = 0;
	struct fscrypt_str fname_crypto_str = FSTR_INIT(NULL, 0), tmp_str;

	dxtrace(printk(KERN_INFO "In htree dirblock_to_tree: block %lu\n",
							(unsigned long)block));
	bh = ext4_read_dirblock(dir, block, DIRENT_HTREE);
	if (IS_ERR(bh))
		return PTR_ERR(bh);

	de = (struct ext4_dir_entry_2 *) bh->b_data;
	top = (struct ext4_dir_entry_2 *) ((char *) de +
					   dir->i_sb->s_blocksize -
					   EXT4_DIR_REC_LEN(0));
	/* Check if the directory is encrypted */
	if (IS_ENCRYPTED(dir)) {
		err = fscrypt_get_encryption_info(dir);
		if (err < 0) {
			brelse(bh);
			return err;
		}
		err = fscrypt_fname_alloc_buffer(dir, EXT4_NAME_LEN,
						     &fname_crypto_str);
		if (err < 0) {
			brelse(bh);
			return err;
		}
	}

	for (; de < top; de = ext4_next_entry(de, dir->i_sb->s_blocksize)) {
		if (ext4_check_dir_entry(dir, NULL, de, bh,
				bh->b_data, bh->b_size,
				(block<<EXT4_BLOCK_SIZE_BITS(dir->i_sb))
					 + ((char *)de - bh->b_data))) {
			/* silently ignore the rest of the block */
			break;
		}
		ext4fs_dirhash(dir, de->name, de->name_len, hinfo);
		if ((hinfo->hash < start_hash) ||
		    ((hinfo->hash == start_hash) &&
		     (hinfo->minor_hash < start_minor_hash)))
			continue;
		if (de->inode == 0)
			continue;
		if (!IS_ENCRYPTED(dir)) {
			tmp_str.name = de->name;
			tmp_str.len = de->name_len;
			err = ext4_htree_store_dirent(dir_file,
				   hinfo->hash, hinfo->minor_hash, de,
				   &tmp_str);
		} else {
			int save_len = fname_crypto_str.len;
			struct fscrypt_str de_name = FSTR_INIT(de->name,
								de->name_len);

			/* Directory is encrypted */
			err = fscrypt_fname_disk_to_usr(dir, hinfo->hash,
					hinfo->minor_hash, &de_name,
					&fname_crypto_str);
			if (err) {
				count = err;
				goto errout;
			}
			err = ext4_htree_store_dirent(dir_file,
				   hinfo->hash, hinfo->minor_hash, de,
					&fname_crypto_str);
			fname_crypto_str.len = save_len;
		}
		if (err != 0) {
			count = err;
			goto errout;
		}
		count++;
	}
errout:
	brelse(bh);
	fscrypt_fname_free_buffer(&fname_crypto_str);
	return count;
}