int ext4_ci_compare(const struct inode *parent, const struct qstr *name,
		    const struct qstr *entry, bool quick)
{
	const struct ext4_sb_info *sbi = EXT4_SB(parent->i_sb);
	const struct unicode_map *um = sbi->s_encoding;
	int ret;

	if (quick)
		ret = utf8_strncasecmp_folded(um, name, entry);
	else
		ret = utf8_strncasecmp(um, name, entry);

	if (ret < 0) {
		/* Handle invalid character sequence as either an error
		 * or as an opaque byte sequence.
		 */
		if (ext4_has_strict_mode(sbi))
			return -EINVAL;

		if (name->len != entry->len)
			return 1;

		return !!memcmp(name->name, entry->name, name->len);
	}

	return ret;
}