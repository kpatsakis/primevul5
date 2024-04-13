rndr_hrule(struct buf *ob, void *opaque)
{
	struct html_renderopt *options = opaque;
	if (ob->size) bufputc(ob, '\n');
	bufputs(ob, USE_XHTML(options) ? "<hr/>\n" : "<hr>\n");
}