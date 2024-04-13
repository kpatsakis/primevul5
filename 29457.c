gstate_to_update(fz_context *ctx, pdf_filter_processor *p)
{
	filter_gstate *gstate = p->gstate;

	/* If we're not the top, that's fine */
	if (gstate->next != NULL)
		return gstate;

	/* We are the top. Push a group, so we're not */
	filter_push(ctx, p);
	gstate = p->gstate;
	gstate->pushed = 1;
	if (p->chain->op_q)
		p->chain->op_q(ctx, p->chain);

	return p->gstate;
}
