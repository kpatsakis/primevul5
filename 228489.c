rndr_quote(struct buf *ob, const struct buf *text, void *opaque)
{
	if (!text || !text->size)
		return 0;

	struct html_renderopt *options = opaque;

	BUFPUTSL(ob, "<q>");

	if (options->flags & HTML_ESCAPE)
		escape_html(ob, text->data, text->size);
	else
		bufput(ob, text->data, text->size);

	BUFPUTSL(ob, "</q>");

	return 1;
}