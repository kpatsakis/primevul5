rndr_codespan(struct buf *ob, const struct buf *text, void *opaque)
{
	struct html_renderopt *options = opaque;
	if (options->flags & HTML_PRETTIFY)
		BUFPUTSL(ob, "<code class=\"prettyprint\">");
	else
		BUFPUTSL(ob, "<code>");
	if (text) escape_html(ob, text->data, text->size);
	BUFPUTSL(ob, "</code>");
	return 1;
}