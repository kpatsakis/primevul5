static int hfsplus_cat_build_record(hfsplus_cat_entry *entry, u32 cnid, struct inode *inode)
{
	if (S_ISDIR(inode->i_mode)) {
		struct hfsplus_cat_folder *folder;

		folder = &entry->folder;
		memset(folder, 0, sizeof(*folder));
		folder->type = cpu_to_be16(HFSPLUS_FOLDER);
		folder->id = cpu_to_be32(inode->i_ino);
		HFSPLUS_I(inode).create_date =
			folder->create_date =
			folder->content_mod_date =
			folder->attribute_mod_date =
			folder->access_date = hfsp_now2mt();
		hfsplus_set_perms(inode, &folder->permissions);
		if (inode == HFSPLUS_SB(inode->i_sb).hidden_dir)
			/* invisible and namelocked */
			folder->user_info.frFlags = cpu_to_be16(0x5000);
		return sizeof(*folder);
	} else {
		struct hfsplus_cat_file *file;

		file = &entry->file;
		memset(file, 0, sizeof(*file));
		file->type = cpu_to_be16(HFSPLUS_FILE);
		file->flags = cpu_to_be16(HFSPLUS_FILE_THREAD_EXISTS);
		file->id = cpu_to_be32(cnid);
		HFSPLUS_I(inode).create_date =
			file->create_date =
			file->content_mod_date =
			file->attribute_mod_date =
			file->access_date = hfsp_now2mt();
		if (cnid == inode->i_ino) {
			hfsplus_set_perms(inode, &file->permissions);
			if (S_ISLNK(inode->i_mode)) {
				file->user_info.fdType = cpu_to_be32(HFSP_SYMLINK_TYPE);
				file->user_info.fdCreator = cpu_to_be32(HFSP_SYMLINK_CREATOR);
			} else {
				file->user_info.fdType = cpu_to_be32(HFSPLUS_SB(inode->i_sb).type);
				file->user_info.fdCreator = cpu_to_be32(HFSPLUS_SB(inode->i_sb).creator);
			}
			if ((file->permissions.rootflags | file->permissions.userflags) & HFSPLUS_FLG_IMMUTABLE)
				file->flags |= cpu_to_be16(HFSPLUS_FILE_LOCKED);
		} else {
			file->user_info.fdType = cpu_to_be32(HFSP_HARDLINK_TYPE);
			file->user_info.fdCreator = cpu_to_be32(HFSP_HFSPLUS_CREATOR);
			file->user_info.fdFlags = cpu_to_be16(0x100);
			file->create_date = HFSPLUS_I(HFSPLUS_SB(inode->i_sb).hidden_dir).create_date;
			file->permissions.dev = cpu_to_be32(HFSPLUS_I(inode).dev);
		}
		return sizeof(*file);
	}
}