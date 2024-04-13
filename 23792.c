void cgit_print_error_page(int code, const char *msg, const char *fmt, ...)
{
	va_list ap;
	ctx.page.expires = ctx.cfg.cache_dynamic_ttl;
	ctx.page.status = code;
	ctx.page.statusmsg = msg;
	cgit_print_http_headers();
	cgit_print_docstart();
	cgit_print_pageheader();
	va_start(ap, fmt);
	cgit_vprint_error(fmt, ap);
	va_end(ap);
	cgit_print_docend();
}