rndr_header(struct buf *ob, const struct buf *text, int level, void *opaque)
{
	struct html_renderopt *options = opaque;

	if (ob->size)
		bufputc(ob, '\n');

	if ((options->flags & HTML_TOC) && level >= options->toc_data.nesting_bounds[0] &&
	     level <= options->toc_data.nesting_bounds[1]) {
		bufprintf(ob, "<h%d id=\"", level);
		rndr_header_anchor(ob, text);
		BUFPUTSL(ob, "\">");
	}
	else
		bufprintf(ob, "<h%d>", level);

	if (text) bufput(ob, text->data, text->size);
	bufprintf(ob, "</h%d>\n", level);
}