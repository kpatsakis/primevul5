toc_link(struct buf *ob, const struct buf *link, const struct buf *title, const struct buf *content, void *opaque)
{
	if (content && content->size)
		bufput(ob, content->data, content->size);
	return 1;
}