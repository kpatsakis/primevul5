static inline void escape_html(struct buf *ob, const uint8_t *source, size_t length)
{
	houdini_escape_html0(ob, source, length, 0);
}