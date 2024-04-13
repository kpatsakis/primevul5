rndr_linebreak(struct buf *ob, void *opaque)
{
	struct html_renderopt *options = opaque;
	bufputs(ob, USE_XHTML(options) ? "<br/>\n" : "<br>\n");
	return 1;
}