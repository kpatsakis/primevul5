static int grep_sha1(struct grep_opt *opt, const unsigned char *sha1, const char *name, int tree_name_len)
{
	unsigned long size;
	char *data;
	enum object_type type;
	char *to_free = NULL;
	int hit;

	data = read_sha1_file(sha1, &type, &size);
	if (!data) {
		error("'%s': unable to read %s", name, sha1_to_hex(sha1));
		return 0;
	}
	if (opt->relative && opt->prefix_length) {
		static char name_buf[PATH_MAX];
		char *cp;
		int name_len = strlen(name) - opt->prefix_length + 1;

		if (!tree_name_len)
			name += opt->prefix_length;
		else {
			if (ARRAY_SIZE(name_buf) <= name_len)
				cp = to_free = xmalloc(name_len);
			else
				cp = name_buf;
			memcpy(cp, name, tree_name_len);
			strcpy(cp + tree_name_len,
			       name + tree_name_len + opt->prefix_length);
			name = cp;
		}
	}
	hit = grep_buffer(opt, name, data, size);
	free(data);
	free(to_free);
	return hit;
}