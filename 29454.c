filter_show_char(fz_context *ctx, pdf_filter_processor *p, int cid, int *unicode)
{
	filter_gstate *gstate = p->gstate;
	pdf_font_desc *fontdesc = gstate->pending.text.font;
	fz_matrix trm;
	int ucsbuf[8];
	int ucslen;
	int remove = 0;

	(void)pdf_tos_make_trm(ctx, &p->tos, &gstate->pending.text, fontdesc, cid, &trm);

	ucslen = 0;
	if (fontdesc->to_unicode)
		ucslen = pdf_lookup_cmap_full(fontdesc->to_unicode, cid, ucsbuf);
	if (ucslen == 0 && (size_t)cid < fontdesc->cid_to_ucs_len)
	{
		ucsbuf[0] = fontdesc->cid_to_ucs[cid];
		ucslen = 1;
	}
	if (ucslen == 0 || (ucslen == 1 && ucsbuf[0] == 0))
	{
		ucsbuf[0] = FZ_REPLACEMENT_CHARACTER;
		ucslen = 1;
	}
	*unicode = ucsbuf[0];

	if (p->text_filter)
	{
		fz_matrix ctm = fz_concat(gstate->sent.ctm, gstate->pending.ctm);
		fz_rect bbox;

		if (fontdesc->wmode == 0)
		{
			bbox.x0 = 0;
			bbox.y0 = fz_font_descender(ctx, fontdesc->font);
			bbox.x1 = fz_advance_glyph(ctx, fontdesc->font, p->tos.gid, 0);
			bbox.y1 = fz_font_ascender(ctx, fontdesc->font);
		}
		else
		{
			fz_rect font_bbox = fz_font_bbox(ctx, fontdesc->font);
			bbox.x0 = font_bbox.x0;
			bbox.x1 = font_bbox.x1;
			bbox.y0 = 0;
			bbox.y1 = fz_advance_glyph(ctx, fontdesc->font, p->tos.gid, 1);
		}

		remove = p->text_filter(ctx, p->opaque, ucsbuf, ucslen, trm, ctm, bbox);
	}

	pdf_tos_move_after_char(ctx, &p->tos);

	return remove;
}
