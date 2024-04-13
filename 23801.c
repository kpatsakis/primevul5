void cgit_print_pageheader(void)
{
	html("<div id='cgit'>");
	if (!ctx.env.authenticated || !ctx.cfg.noheader)
		print_header();

	html("<table class='tabs'><tr><td>\n");
	if (ctx.env.authenticated && ctx.repo) {
		if (ctx.repo->readme.nr)
			reporevlink("about", "about", NULL,
				    hc("about"), ctx.qry.head, NULL,
				    NULL);
		cgit_summary_link("summary", NULL, hc("summary"),
				  ctx.qry.head);
		cgit_refs_link("refs", NULL, hc("refs"), ctx.qry.head,
			       ctx.qry.sha1, NULL);
		cgit_log_link("log", NULL, hc("log"), ctx.qry.head,
			      NULL, ctx.qry.vpath, 0, NULL, NULL,
			      ctx.qry.showmsg, ctx.qry.follow);
		cgit_tree_link("tree", NULL, hc("tree"), ctx.qry.head,
			       ctx.qry.sha1, ctx.qry.vpath);
		cgit_commit_link("commit", NULL, hc("commit"),
				 ctx.qry.head, ctx.qry.sha1, ctx.qry.vpath);
		cgit_diff_link("diff", NULL, hc("diff"), ctx.qry.head,
			       ctx.qry.sha1, ctx.qry.sha2, ctx.qry.vpath);
		if (ctx.repo->max_stats)
			cgit_stats_link("stats", NULL, hc("stats"),
					ctx.qry.head, ctx.qry.vpath);
		html("</td><td class='form'>");
		html("<form class='right' method='get' action='");
		if (ctx.cfg.virtual_root) {
			char *fileurl = cgit_fileurl(ctx.qry.repo, "log",
						   ctx.qry.vpath, NULL);
			html_url_path(fileurl);
			free(fileurl);
		}
		html("'>\n");
		cgit_add_hidden_formfields(1, 0, "log");
		html("<select name='qt'>\n");
		html_option("grep", "log msg", ctx.qry.grep);
		html_option("author", "author", ctx.qry.grep);
		html_option("committer", "committer", ctx.qry.grep);
		html_option("range", "range", ctx.qry.grep);
		html("</select>\n");
		html("<input class='txt' type='text' size='10' name='q' value='");
		html_attr(ctx.qry.search);
		html("'/>\n");
		html("<input type='submit' value='search'/>\n");
		html("</form>\n");
	} else if (ctx.env.authenticated) {
		char *currenturl = cgit_currenturl();
		site_link(NULL, "index", NULL, hc("repolist"), NULL, NULL, 0, 1);
		if (ctx.cfg.root_readme)
			site_link("about", "about", NULL, hc("about"),
				  NULL, NULL, 0, 1);
		html("</td><td class='form'>");
		html("<form method='get' action='");
		html_attr(currenturl);
		html("'>\n");
		html("<input type='text' name='q' size='10' value='");
		html_attr(ctx.qry.search);
		html("'/>\n");
		html("<input type='submit' value='search'/>\n");
		html("</form>");
		free(currenturl);
	}
	html("</td></tr></table>\n");
	if (ctx.env.authenticated && ctx.qry.vpath) {
		html("<div class='path'>");
		html("path: ");
		cgit_print_path_crumbs(ctx.qry.vpath);
		if (ctx.cfg.enable_follow_links && !strcmp(ctx.qry.page, "log")) {
			html(" (");
			ctx.qry.follow = !ctx.qry.follow;
			cgit_self_link(ctx.qry.follow ? "follow" : "unfollow",
					NULL, NULL);
			ctx.qry.follow = !ctx.qry.follow;
			html(")");
		}
		html("</div>");
	}
	html("<div class='content'>");
}