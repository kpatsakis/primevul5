int hfsplus_delete_cat(u32 cnid, struct inode *dir, struct qstr *str)
{
	struct super_block *sb;
	struct hfs_find_data fd;
	struct hfsplus_fork_raw fork;
	struct list_head *pos;
	int err, off;
	u16 type;

	dprint(DBG_CAT_MOD, "delete_cat: %s,%u\n", str ? str->name : NULL, cnid);
	sb = dir->i_sb;
	hfs_find_init(HFSPLUS_SB(sb).cat_tree, &fd);

	if (!str) {
		int len;

		hfsplus_cat_build_key(sb, fd.search_key, cnid, NULL);
		err = hfs_brec_find(&fd);
		if (err)
			goto out;

		off = fd.entryoffset + offsetof(struct hfsplus_cat_thread, nodeName);
		fd.search_key->cat.parent = cpu_to_be32(dir->i_ino);
		hfs_bnode_read(fd.bnode, &fd.search_key->cat.name.length, off, 2);
		len = be16_to_cpu(fd.search_key->cat.name.length) * 2;
		hfs_bnode_read(fd.bnode, &fd.search_key->cat.name.unicode, off + 2, len);
		fd.search_key->key_len = cpu_to_be16(6 + len);
	} else
		hfsplus_cat_build_key(sb, fd.search_key, dir->i_ino, str);

	err = hfs_brec_find(&fd);
	if (err)
		goto out;

	type = hfs_bnode_read_u16(fd.bnode, fd.entryoffset);
	if (type == HFSPLUS_FILE) {
#if 0
		off = fd.entryoffset + offsetof(hfsplus_cat_file, data_fork);
		hfs_bnode_read(fd.bnode, &fork, off, sizeof(fork));
		hfsplus_free_fork(sb, cnid, &fork, HFSPLUS_TYPE_DATA);
#endif

		off = fd.entryoffset + offsetof(struct hfsplus_cat_file, rsrc_fork);
		hfs_bnode_read(fd.bnode, &fork, off, sizeof(fork));
		hfsplus_free_fork(sb, cnid, &fork, HFSPLUS_TYPE_RSRC);
	}

	list_for_each(pos, &HFSPLUS_I(dir).open_dir_list) {
		struct hfsplus_readdir_data *rd =
			list_entry(pos, struct hfsplus_readdir_data, list);
		if (fd.tree->keycmp(fd.search_key, (void *)&rd->key) < 0)
			rd->file->f_pos--;
	}

	err = hfs_brec_remove(&fd);
	if (err)
		goto out;

	hfsplus_cat_build_key(sb, fd.search_key, cnid, NULL);
	err = hfs_brec_find(&fd);
	if (err)
		goto out;

	err = hfs_brec_remove(&fd);
	if (err)
		goto out;

	dir->i_size--;
	dir->i_mtime = dir->i_ctime = CURRENT_TIME_SEC;
	mark_inode_dirty(dir);
out:
	hfs_find_exit(&fd);

	return err;
}