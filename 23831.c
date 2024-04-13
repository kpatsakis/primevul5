char *cgit_hosturl(void)
{
	if (ctx.env.http_host)
		return xstrdup(ctx.env.http_host);
	if (!ctx.env.server_name)
		return NULL;
	if (!ctx.env.server_port || atoi(ctx.env.server_port) == 80)
		return xstrdup(ctx.env.server_name);
	return fmtalloc("%s:%s", ctx.env.server_name, ctx.env.server_port);
}