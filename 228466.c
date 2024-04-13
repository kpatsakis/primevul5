rndr_listitem(struct buf *ob, const struct buf *text, int flags, void *opaque)
{
	BUFPUTSL(ob, "<li>");
	if (text) {
		size_t size = text->size;
		while (size && text->data[size - 1] == '\n')
			size--;

		bufput(ob, text->data, size);
	}
	BUFPUTSL(ob, "</li>\n");
}