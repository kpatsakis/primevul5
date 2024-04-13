static void flush_tags(fz_context *ctx, pdf_filter_processor *p, tag_record **tags)
{
	tag_record *tag = *tags;

	if (tag == NULL)
		return;
	if (tag->prev)
		flush_tags(ctx, p, &tag->prev);
	if (tag->bdc)
	{
		if (p->chain->op_BDC)
			p->chain->op_BDC(ctx, p->chain, tag->tag, tag->raw, tag->cooked);
	}
	else if (p->chain->op_BMC)
		p->chain->op_BMC(ctx, p->chain, tag->tag);
	tag->prev = p->current_tags;
	p->current_tags = tag;
	*tags = NULL;
}
