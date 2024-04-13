rndr_footnotes(struct buf *ob, const struct buf *text, void *opaque)
{
	struct html_renderopt *options = opaque;

	if (ob->size) bufputc(ob, '\n');

	BUFPUTSL(ob, "<div class=\"footnotes\">\n");
	bufputs(ob, USE_XHTML(options) ? "<hr/>\n" : "<hr>\n");
	BUFPUTSL(ob, "<ol>\n");

	if (text)
		bufput(ob, text->data, text->size);

	BUFPUTSL(ob, "\n</ol>\n</div>\n");
}