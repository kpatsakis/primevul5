rndr_blockcode(struct buf *ob, const struct buf *text, const struct buf *lang, void *opaque)
{
	struct html_renderopt *options = opaque;

	if (ob->size) bufputc(ob, '\n');

	if (lang && lang->size) {
		size_t i, cls;
		if (options->flags & HTML_PRETTIFY) {
			BUFPUTSL(ob, "<pre><code class=\"prettyprint lang-");
			cls++;
		} else {
			BUFPUTSL(ob, "<pre><code class=\"");
		}

		for (i = 0, cls = 0; i < lang->size; ++i, ++cls) {
			while (i < lang->size && isspace(lang->data[i]))
				i++;

			if (i < lang->size) {
				size_t org = i;
				while (i < lang->size && !isspace(lang->data[i]))
					i++;

				if (lang->data[org] == '.')
					org++;

				if (cls) bufputc(ob, ' ');
				escape_html(ob, lang->data + org, i - org);
			}
		}

		BUFPUTSL(ob, "\">");
	} else if (options->flags & HTML_PRETTIFY) {
		BUFPUTSL(ob, "<pre><code class=\"prettyprint\">");
	} else {
		BUFPUTSL(ob, "<pre><code>");
	}

	if (text)
		escape_html(ob, text->data, text->size);

	BUFPUTSL(ob, "</code></pre>\n");
}