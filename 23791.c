void cgit_add_hidden_formfields(int incl_head, int incl_search,
				const char *page)
{
	if (!ctx.cfg.virtual_root) {
		struct strbuf url = STRBUF_INIT;

		strbuf_addf(&url, "%s/%s", ctx.qry.repo, page);
		if (ctx.qry.vpath)
			strbuf_addf(&url, "/%s", ctx.qry.vpath);
		html_hidden("url", url.buf);
		strbuf_release(&url);
	}

	if (incl_head && ctx.qry.head && ctx.repo->defbranch &&
	    strcmp(ctx.qry.head, ctx.repo->defbranch))
		html_hidden("h", ctx.qry.head);

	if (ctx.qry.sha1)
		html_hidden("id", ctx.qry.sha1);
	if (ctx.qry.sha2)
		html_hidden("id2", ctx.qry.sha2);
	if (ctx.qry.showmsg)
		html_hidden("showmsg", "1");

	if (incl_search) {
		if (ctx.qry.grep)
			html_hidden("qt", ctx.qry.grep);
		if (ctx.qry.search)
			html_hidden("q", ctx.qry.search);
	}
}