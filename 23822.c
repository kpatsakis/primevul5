void cgit_print_docstart(void)
{
	if (ctx.cfg.embedded) {
		if (ctx.cfg.header)
			html_include(ctx.cfg.header);
		return;
	}

	char *host = cgit_hosturl();
	html(cgit_doctype);
	html("<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='en' lang='en'>\n");
	html("<head>\n");
	html("<title>");
	html_txt(ctx.page.title);
	html("</title>\n");
	htmlf("<meta name='generator' content='cgit %s'/>\n", cgit_version);
	if (ctx.cfg.robots && *ctx.cfg.robots)
		htmlf("<meta name='robots' content='%s'/>\n", ctx.cfg.robots);
	html("<link rel='stylesheet' type='text/css' href='");
	html_attr(ctx.cfg.css);
	html("'/>\n");
	if (ctx.cfg.favicon) {
		html("<link rel='shortcut icon' href='");
		html_attr(ctx.cfg.favicon);
		html("'/>\n");
	}
	if (host && ctx.repo && ctx.qry.head) {
		char *fileurl;
		struct strbuf sb = STRBUF_INIT;
		strbuf_addf(&sb, "h=%s", ctx.qry.head);

		html("<link rel='alternate' title='Atom feed' href='");
		html(cgit_httpscheme());
		html_attr(host);
		fileurl = cgit_fileurl(ctx.repo->url, "atom", ctx.qry.vpath,
				       sb.buf);
		html_attr(fileurl);
		html("' type='application/atom+xml'/>\n");
		strbuf_release(&sb);
		free(fileurl);
	}
	if (ctx.repo)
		cgit_add_clone_urls(print_rel_vcs_link);
	if (ctx.cfg.head_include)
		html_include(ctx.cfg.head_include);
	html("</head>\n");
	html("<body>\n");
	if (ctx.cfg.header)
		html_include(ctx.cfg.header);
	free(host);
}