const char *cgit_rooturl(void)
{
	if (ctx.cfg.virtual_root)
		return ctx.cfg.virtual_root;
	else
		return ctx.cfg.script_name;
}