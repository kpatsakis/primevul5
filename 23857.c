void cgit_print_snapshot_links(const char *repo, const char *head,
			       const char *hex, int snapshots)
{
	const struct cgit_snapshot_format* f;
	struct strbuf filename = STRBUF_INIT;
	size_t prefixlen;
	unsigned char sha1[20];

	if (get_sha1(fmt("refs/tags/%s", hex), sha1) == 0 &&
	    (hex[0] == 'v' || hex[0] == 'V') && isdigit(hex[1]))
		hex++;
	strbuf_addf(&filename, "%s-%s", cgit_repobasename(repo), hex);
	prefixlen = filename.len;
	for (f = cgit_snapshot_formats; f->suffix; f++) {
		if (!(snapshots & f->bit))
			continue;
		strbuf_setlen(&filename, prefixlen);
		strbuf_addstr(&filename, f->suffix);
		cgit_snapshot_link(filename.buf, NULL, NULL, NULL, NULL,
				   filename.buf);
		html("<br/>");
	}
	strbuf_release(&filename);
}