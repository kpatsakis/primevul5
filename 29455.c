filter_show_space(fz_context *ctx, pdf_filter_processor *p, float tadj)
{
	filter_gstate *gstate = p->gstate;
	pdf_font_desc *fontdesc = gstate->pending.text.font;

	if (fontdesc->wmode == 0)
		p->tos.tm = fz_pre_translate(p->tos.tm, tadj * gstate->pending.text.scale, 0);
	else
		p->tos.tm = fz_pre_translate(p->tos.tm, 0, tadj);
}
