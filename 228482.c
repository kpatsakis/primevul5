toc_header(struct buf *ob, const struct buf *text, int level, void *opaque)
{
	struct html_renderopt *options = opaque;

	if (level >= options->toc_data.nesting_bounds[0] &&
	    level <= options->toc_data.nesting_bounds[1]) {
		/* set the level offset if this is the first header
		 * we're parsing for the document */
		if (options->toc_data.current_level == 0)
			options->toc_data.level_offset = level - 1;

		level -= options->toc_data.level_offset;

		if (level > options->toc_data.current_level) {
			while (level > options->toc_data.current_level) {
				BUFPUTSL(ob, "<ul>\n<li>\n");
				options->toc_data.current_level++;
			}
		} else if (level < options->toc_data.current_level) {
			BUFPUTSL(ob, "</li>\n");
			while (level < options->toc_data.current_level) {
				BUFPUTSL(ob, "</ul>\n</li>\n");
				options->toc_data.current_level--;
			}
			BUFPUTSL(ob,"<li>\n");
		} else {
			BUFPUTSL(ob,"</li>\n<li>\n");
		}

		bufprintf(ob, "<a href=\"#");
		rndr_header_anchor(ob, text);
		BUFPUTSL(ob, "\">");

		if (text) {
			if (options->flags & HTML_ESCAPE)
				escape_html(ob, text->data, text->size);
			else
				bufput(ob, text->data, text->size);
		}

		BUFPUTSL(ob, "</a>\n");
	}
}