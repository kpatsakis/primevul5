static void cgit_self_link(char *name, const char *title, const char *class)
{
	if (!strcmp(ctx.qry.page, "repolist"))
		cgit_index_link(name, title, class, ctx.qry.search, ctx.qry.sort,
				ctx.qry.ofs, 1);
	else if (!strcmp(ctx.qry.page, "summary"))
		cgit_summary_link(name, title, class, ctx.qry.head);
	else if (!strcmp(ctx.qry.page, "tag"))
		cgit_tag_link(name, title, class, ctx.qry.has_sha1 ?
			       ctx.qry.sha1 : ctx.qry.head);
	else if (!strcmp(ctx.qry.page, "tree"))
		cgit_tree_link(name, title, class, ctx.qry.head,
			       ctx.qry.has_sha1 ? ctx.qry.sha1 : NULL,
			       ctx.qry.path);
	else if (!strcmp(ctx.qry.page, "plain"))
		cgit_plain_link(name, title, class, ctx.qry.head,
				ctx.qry.has_sha1 ? ctx.qry.sha1 : NULL,
				ctx.qry.path);
	else if (!strcmp(ctx.qry.page, "log"))
		cgit_log_link(name, title, class, ctx.qry.head,
			      ctx.qry.has_sha1 ? ctx.qry.sha1 : NULL,
			      ctx.qry.path, ctx.qry.ofs,
			      ctx.qry.grep, ctx.qry.search,
			      ctx.qry.showmsg, ctx.qry.follow);
	else if (!strcmp(ctx.qry.page, "commit"))
		cgit_commit_link(name, title, class, ctx.qry.head,
				 ctx.qry.has_sha1 ? ctx.qry.sha1 : NULL,
				 ctx.qry.path);
	else if (!strcmp(ctx.qry.page, "patch"))
		cgit_patch_link(name, title, class, ctx.qry.head,
				ctx.qry.has_sha1 ? ctx.qry.sha1 : NULL,
				ctx.qry.path);
	else if (!strcmp(ctx.qry.page, "refs"))
		cgit_refs_link(name, title, class, ctx.qry.head,
			       ctx.qry.has_sha1 ? ctx.qry.sha1 : NULL,
			       ctx.qry.path);
	else if (!strcmp(ctx.qry.page, "snapshot"))
		cgit_snapshot_link(name, title, class, ctx.qry.head,
				   ctx.qry.has_sha1 ? ctx.qry.sha1 : NULL,
				   ctx.qry.path);
	else if (!strcmp(ctx.qry.page, "diff"))
		cgit_diff_link(name, title, class, ctx.qry.head,
			       ctx.qry.sha1, ctx.qry.sha2,
			       ctx.qry.path);
	else if (!strcmp(ctx.qry.page, "stats"))
		cgit_stats_link(name, title, class, ctx.qry.head,
				ctx.qry.path);
	else {
		/* Don't known how to make link for this page */
		repolink(title, class, ctx.qry.page, ctx.qry.head, ctx.qry.path);
		html("><!-- cgit_self_link() doesn't know how to make link for page '");
		html_txt(ctx.qry.page);
		html("' -->");
		html_txt(name);
		html("</a>");
	}
}