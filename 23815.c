static void add_clone_urls(void (*fn)(const char *), char *txt, char *suffix)
{
	struct strbuf **url_list = strbuf_split_str(txt, ' ', 0);
	int i;

	for (i = 0; url_list[i]; i++) {
		strbuf_rtrim(url_list[i]);
		if (url_list[i]->len == 0)
			continue;
		if (suffix && *suffix)
			strbuf_addf(url_list[i], "/%s", suffix);
		fn(url_list[i]->buf);
	}

	strbuf_list_free(url_list);
}