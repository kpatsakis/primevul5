static int grep_tree(struct grep_opt *opt, const char **paths,
		     struct tree_desc *tree,
		     const char *tree_name, const char *base)
{
	int len;
	int hit = 0;
	struct name_entry entry;
	char *down;
	int tn_len = strlen(tree_name);
	struct strbuf pathbuf;

	strbuf_init(&pathbuf, PATH_MAX + tn_len);

	if (tn_len) {
		strbuf_add(&pathbuf, tree_name, tn_len);
		strbuf_addch(&pathbuf, ':');
		tn_len = pathbuf.len;
	}
	strbuf_addstr(&pathbuf, base);
	len = pathbuf.len;

	while (tree_entry(tree, &entry)) {
		int te_len = tree_entry_len(entry.path, entry.sha1);
		pathbuf.len = len;
		strbuf_add(&pathbuf, entry.path, te_len);

		if (S_ISDIR(entry.mode))
			/* Match "abc/" against pathspec to
			 * decide if we want to descend into "abc"
			 * directory.
			 */
			strbuf_addch(&pathbuf, '/');

		down = pathbuf.buf + tn_len;
		if (!pathspec_matches(paths, down))
			;
		else if (S_ISREG(entry.mode))
			hit |= grep_sha1(opt, entry.sha1, pathbuf.buf, tn_len);
		else if (S_ISDIR(entry.mode)) {
			enum object_type type;
			struct tree_desc sub;
			void *data;
			unsigned long size;

			data = read_sha1_file(entry.sha1, &type, &size);
			if (!data)
				die("unable to read tree (%s)",
				    sha1_to_hex(entry.sha1));
			init_tree_desc(&sub, data, size);
			hit |= grep_tree(opt, paths, &sub, tree_name, down);
			free(data);
		}
	}
	strbuf_release(&pathbuf);
	return hit;
}