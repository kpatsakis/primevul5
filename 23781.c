static void print_header(void)
{
	char *logo = NULL, *logo_link = NULL;

	html("<table id='header'>\n");
	html("<tr>\n");

	if (ctx.repo && ctx.repo->logo && *ctx.repo->logo)
		logo = ctx.repo->logo;
	else
		logo = ctx.cfg.logo;
	if (ctx.repo && ctx.repo->logo_link && *ctx.repo->logo_link)
		logo_link = ctx.repo->logo_link;
	else
		logo_link = ctx.cfg.logo_link;
	if (logo && *logo) {
		html("<td class='logo' rowspan='2'><a href='");
		if (logo_link && *logo_link)
			html_attr(logo_link);
		else
			html_attr(cgit_rooturl());
		html("'><img src='");
		html_attr(logo);
		html("' alt='cgit logo'/></a></td>\n");
	}

	html("<td class='main'>");
	if (ctx.repo) {
		cgit_index_link("index", NULL, NULL, NULL, NULL, 0, 1);
		html(" : ");
		cgit_summary_link(ctx.repo->name, ctx.repo->name, NULL, NULL);
		if (ctx.env.authenticated) {
			html("</td><td class='form'>");
			html("<form method='get' action=''>\n");
			cgit_add_hidden_formfields(0, 1, ctx.qry.page);
			html("<select name='h' onchange='this.form.submit();'>\n");
			for_each_branch_ref(print_branch_option, ctx.qry.head);
			if (ctx.repo->enable_remote_branches)
				for_each_remote_ref(print_branch_option, ctx.qry.head);
			html("</select> ");
			html("<input type='submit' name='' value='switch'/>");
			html("</form>");
		}
	} else
		html_txt(ctx.cfg.root_title);
	html("</td></tr>\n");

	html("<tr><td class='sub'>");
	if (ctx.repo) {
		html_txt(ctx.repo->desc);
		html("</td><td class='sub right'>");
		html_txt(ctx.repo->owner);
	} else {
		if (ctx.cfg.root_desc)
			html_txt(ctx.cfg.root_desc);
		else if (ctx.cfg.index_info)
			html_include(ctx.cfg.index_info);
	}
	html("</td></tr></table>\n");
}