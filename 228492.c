rndr_raw_block(struct buf *ob, const struct buf *text, void *opaque)
{
	size_t org, size;
	struct html_renderopt *options = opaque;

	if (!text)
		return;

	size = text->size;
	while (size > 0 && text->data[size - 1] == '\n')
		size--;

	for (org = 0; org < size && text->data[org] == '\n'; ++org)

	if (org >= size)
		return;

	/* Remove style tags if the `:no_styles` option is enabled */
	if ((options->flags & HTML_SKIP_STYLE) != 0 &&
		sdhtml_is_tag(text->data, size, "style"))
		return;

	if (ob->size)
		bufputc(ob, '\n');

	bufput(ob, text->data + org, size - org);
	bufputc(ob, '\n');
}