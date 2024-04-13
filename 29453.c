filter_push(fz_context *ctx, pdf_filter_processor *p)
{
	filter_gstate *gstate = p->gstate;
	filter_gstate *new_gstate = fz_malloc_struct(ctx, filter_gstate);
	*new_gstate = *gstate;
	new_gstate->pushed = 0;
	new_gstate->next = gstate;
	p->gstate = new_gstate;

	pdf_keep_font(ctx, new_gstate->pending.text.font);
	pdf_keep_font(ctx, new_gstate->sent.text.font);
}
