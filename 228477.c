rndr_footnote_ref(struct buf *ob, unsigned int num, void *opaque)
{
	bufprintf(ob, "<sup id=\"fnref%d\"><a href=\"#fn%d\">%d</a></sup>", num, num, num);
	return 1;
}