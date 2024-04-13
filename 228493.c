rndr_normal_text(struct buf *ob, const struct buf *text, void *opaque)
{
	if (text)
		escape_html(ob, text->data, text->size);
}