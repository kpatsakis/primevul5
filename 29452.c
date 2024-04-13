filter_pop(fz_context *ctx, pdf_filter_processor *p)
{
	filter_gstate *gstate = p->gstate;
	filter_gstate *old = gstate->next;

	/* We are at the top, so nothing to pop! */
	if (old == NULL)
		return 1;

	if (gstate->pushed)
		if (p->chain->op_Q)
			p->chain->op_Q(ctx, p->chain);

	pdf_drop_font(ctx, gstate->pending.text.font);
	pdf_drop_font(ctx, gstate->sent.text.font);
	fz_free(ctx, gstate);
	p->gstate = old;
	return 0;
}
