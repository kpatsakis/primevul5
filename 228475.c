rndr_highlight(struct buf *ob, const struct buf *text, void *opaque)
{
	if (!text || !text->size)
		return 0;

	BUFPUTSL(ob, "<mark>");
	bufput(ob, text->data, text->size);
	BUFPUTSL(ob, "</mark>");

	return 1;
}