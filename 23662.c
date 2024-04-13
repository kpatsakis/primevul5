void hfsplus_cat_build_key(struct super_block *sb, hfsplus_btree_key *key,
			   u32 parent, struct qstr *str)
{
	int len;

	key->cat.parent = cpu_to_be32(parent);
	if (str) {
		hfsplus_asc2uni(sb, &key->cat.name, str->name, str->len);
		len = be16_to_cpu(key->cat.name.length);
	} else {
		key->cat.name.length = 0;
		len = 0;
	}
	key->key_len = cpu_to_be16(6 + 2 * len);
}