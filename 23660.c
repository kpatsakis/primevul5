static int hfsplus_fill_cat_thread(struct super_block *sb,
				   hfsplus_cat_entry *entry, int type,
				   u32 parentid, struct qstr *str)
{
	entry->type = cpu_to_be16(type);
	entry->thread.reserved = 0;
	entry->thread.parentID = cpu_to_be32(parentid);
	hfsplus_asc2uni(sb, &entry->thread.nodeName, str->name, str->len);
	return 10 + be16_to_cpu(entry->thread.nodeName.length) * 2;
}