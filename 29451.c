static void filter_flush(fz_context *ctx, pdf_filter_processor *p, int flush)
{
	filter_gstate *gstate = gstate_to_update(ctx, p);
	int i;

	if (gstate->pushed == 0)
	{
		gstate->pushed = 1;
		if (p->chain->op_q)
			p->chain->op_q(ctx, p->chain);
	}

	if (flush)
		flush_tags(ctx, p, &p->pending_tags);

	if (flush & FLUSH_CTM)
	{
		if (gstate->pending.ctm.a != 1 || gstate->pending.ctm.b != 0 ||
			gstate->pending.ctm.c != 0 || gstate->pending.ctm.d != 1 ||
			gstate->pending.ctm.e != 0 || gstate->pending.ctm.f != 0)
		{
			fz_matrix current = gstate->sent.ctm;

			if (p->chain->op_cm)
				p->chain->op_cm(ctx, p->chain,
					gstate->pending.ctm.a,
					gstate->pending.ctm.b,
					gstate->pending.ctm.c,
					gstate->pending.ctm.d,
					gstate->pending.ctm.e,
					gstate->pending.ctm.f);

			gstate->sent.ctm = fz_concat(current, gstate->pending.ctm);
			gstate->pending.ctm.a = 1;
			gstate->pending.ctm.b = 0;
			gstate->pending.ctm.c = 0;
			gstate->pending.ctm.d = 1;
			gstate->pending.ctm.e = 0;
			gstate->pending.ctm.f = 0;
		}
	}

	if (flush & FLUSH_COLOR_F)
	{
		if (gstate->pending.cs.cs == fz_device_gray(ctx) && !gstate->pending.sc.pat && !gstate->pending.sc.shd && gstate->pending.sc.n == 1 &&
			(gstate->sent.cs.cs != fz_device_gray(ctx) || gstate->sent.sc.pat || gstate->sent.sc.shd || gstate->sent.sc.n != 1 || gstate->pending.sc.c[0] != gstate->sent.sc.c[0]))
		{
			if (p->chain->op_g)
				p->chain->op_g(ctx, p->chain, gstate->pending.sc.c[0]);
			goto done_sc;
		}
		if (gstate->pending.cs.cs == fz_device_rgb(ctx) && !gstate->pending.sc.pat && !gstate->pending.sc.shd && gstate->pending.sc.n == 3 &&
			(gstate->sent.cs.cs != fz_device_rgb(ctx) || gstate->sent.sc.pat || gstate->sent.sc.shd || gstate->sent.sc.n != 3 || gstate->pending.sc.c[0] != gstate->sent.sc.c[0] ||
				gstate->pending.sc.c[1] != gstate->sent.sc.c[1] || gstate->pending.sc.c[1] != gstate->sent.sc.c[1]))
		{
			if (p->chain->op_rg)
				p->chain->op_rg(ctx, p->chain, gstate->pending.sc.c[0], gstate->pending.sc.c[1], gstate->pending.sc.c[2]);
			goto done_sc;
		}
		if (gstate->pending.cs.cs == fz_device_cmyk(ctx) && !gstate->pending.sc.pat && !gstate->pending.sc.shd && gstate->pending.sc.n == 4 &&
			(gstate->sent.cs.cs != fz_device_cmyk(ctx) || gstate->sent.sc.pat || gstate->sent.sc.shd || gstate->pending.sc.n != 4 || gstate->pending.sc.c[0] != gstate->sent.sc.c[0] ||
				gstate->pending.sc.c[1] != gstate->sent.sc.c[1] || gstate->pending.sc.c[2] != gstate->sent.sc.c[2] || gstate->pending.sc.c[3] != gstate->sent.sc.c[3]))
		{
			if (p->chain->op_k)
				p->chain->op_k(ctx, p->chain, gstate->pending.sc.c[0], gstate->pending.sc.c[1], gstate->pending.sc.c[2], gstate->pending.sc.c[3]);
			goto done_sc;
		}

		if (strcmp(gstate->pending.cs.name, gstate->sent.cs.name))
		{
			if (p->chain->op_cs)
				p->chain->op_cs(ctx, p->chain, gstate->pending.cs.name, gstate->pending.cs.cs);
		}

		/* pattern or shading */
		if (gstate->pending.sc.name[0])
		{
			int emit = 0;
			if (strcmp(gstate->pending.sc.name, gstate->sent.sc.name))
				emit = 1;
			if (gstate->pending.sc.n != gstate->sent.sc.n)
				emit = 1;
			else
				for (i = 0; i < gstate->pending.sc.n; ++i)
					if (gstate->pending.sc.c[i] != gstate->sent.sc.c[i])
						emit = 1;
			if (emit)
			{
				if (gstate->pending.sc.pat)
					if (p->chain->op_sc_pattern)
						p->chain->op_sc_pattern(ctx, p->chain, gstate->pending.sc.name, gstate->pending.sc.pat, gstate->pending.sc.n, gstate->pending.sc.c);
				if (gstate->pending.sc.shd)
					if (p->chain->op_sc_shade)
						p->chain->op_sc_shade(ctx, p->chain, gstate->pending.sc.name, gstate->pending.sc.shd);
			}
		}

		/* plain color */
		else
		{
			int emit = 0;
			if (gstate->pending.sc.n != gstate->sent.sc.n)
				emit = 1;
			else
				for (i = 0; i < gstate->pending.sc.n; ++i)
					if (gstate->pending.sc.c[i] != gstate->sent.sc.c[i])
						emit = 1;
			if (emit)
			{
				if (p->chain->op_sc_color)
					p->chain->op_sc_color(ctx, p->chain, gstate->pending.sc.n, gstate->pending.sc.c);
			}
		}

done_sc:
		gstate->sent.cs = gstate->pending.cs;
		gstate->sent.sc = gstate->pending.sc;
	}

	if (flush & FLUSH_COLOR_S)
	{
		if (gstate->pending.CS.cs == fz_device_gray(ctx) && !gstate->pending.SC.pat && !gstate->pending.SC.shd && gstate->pending.SC.n == 1 &&
			(gstate->sent.CS.cs != fz_device_gray(ctx) || gstate->sent.SC.pat || gstate->sent.SC.shd || gstate->sent.SC.n != 0 || gstate->pending.SC.c[0] != gstate->sent.SC.c[0]))
		{
			if (p->chain->op_G)
				p->chain->op_G(ctx, p->chain, gstate->pending.SC.c[0]);
			goto done_SC;
		}
		if (gstate->pending.CS.cs == fz_device_rgb(ctx) && !gstate->pending.SC.pat && !gstate->pending.SC.shd && gstate->pending.SC.n == 3 &&
			(gstate->sent.CS.cs != fz_device_rgb(ctx) || gstate->sent.SC.pat || gstate->sent.SC.shd || gstate->sent.SC.n != 3 || gstate->pending.SC.c[0] != gstate->sent.SC.c[0] ||
				gstate->pending.SC.c[1] != gstate->sent.SC.c[1] || gstate->pending.SC.c[1] != gstate->sent.SC.c[1]))
		{
			if (p->chain->op_RG)
				p->chain->op_RG(ctx, p->chain, gstate->pending.SC.c[0], gstate->pending.SC.c[1], gstate->pending.SC.c[2]);
			goto done_SC;
		}
		if (gstate->pending.CS.cs == fz_device_cmyk(ctx) && !gstate->pending.SC.pat && !gstate->pending.SC.shd && gstate->pending.SC.n == 4 &&
			(gstate->sent.CS.cs != fz_device_cmyk(ctx) || gstate->sent.SC.pat || gstate->sent.SC.shd || gstate->pending.SC.n != 4 || gstate->pending.SC.c[0] != gstate->sent.SC.c[0] ||
				gstate->pending.SC.c[1] != gstate->sent.SC.c[1] || gstate->pending.SC.c[2] != gstate->sent.SC.c[2] || gstate->pending.SC.c[3] != gstate->sent.SC.c[3]))
		{
			if (p->chain->op_K)
				p->chain->op_K(ctx, p->chain, gstate->pending.SC.c[0], gstate->pending.SC.c[1], gstate->pending.SC.c[2], gstate->pending.SC.c[3]);
			goto done_SC;
		}

		if (strcmp(gstate->pending.CS.name, gstate->sent.CS.name))
		{
			if (p->chain->op_CS)
				p->chain->op_CS(ctx, p->chain, gstate->pending.CS.name, gstate->pending.CS.cs);
		}

		/* pattern or shading */
		if (gstate->pending.SC.name[0])
		{
			int emit = 0;
			if (strcmp(gstate->pending.SC.name, gstate->sent.SC.name))
				emit = 1;
			if (gstate->pending.SC.n != gstate->sent.SC.n)
				emit = 1;
			else
				for (i = 0; i < gstate->pending.SC.n; ++i)
					if (gstate->pending.SC.c[i] != gstate->sent.SC.c[i])
						emit = 1;
			if (emit)
			{
				if (gstate->pending.SC.pat)
					if (p->chain->op_SC_pattern)
						p->chain->op_SC_pattern(ctx, p->chain, gstate->pending.SC.name, gstate->pending.SC.pat, gstate->pending.SC.n, gstate->pending.SC.c);
				if (gstate->pending.SC.shd)
					if (p->chain->op_SC_shade)
						p->chain->op_SC_shade(ctx, p->chain, gstate->pending.SC.name, gstate->pending.SC.shd);
			}
		}

		/* plain color */
		else
		{
			int emit = 0;
			if (gstate->pending.SC.n != gstate->sent.SC.n)
				emit = 1;
			else
				for (i = 0; i < gstate->pending.SC.n; ++i)
					if (gstate->pending.SC.c[i] != gstate->sent.SC.c[i])
						emit = 1;
			if (emit)
			{
				if (p->chain->op_SC_color)
					p->chain->op_SC_color(ctx, p->chain, gstate->pending.SC.n, gstate->pending.SC.c);
			}
		}

done_SC:
		gstate->sent.CS = gstate->pending.CS;
		gstate->sent.SC = gstate->pending.SC;
	}

	if (flush & FLUSH_STROKE)
	{
		if (gstate->pending.stroke.linecap != gstate->sent.stroke.linecap)
		{
			if (p->chain->op_J)
				p->chain->op_J(ctx, p->chain, gstate->pending.stroke.linecap);
		}
		if (gstate->pending.stroke.linejoin != gstate->sent.stroke.linejoin)
		{
			if (p->chain->op_j)
				p->chain->op_j(ctx, p->chain, gstate->pending.stroke.linejoin);
		}
		if (gstate->pending.stroke.linewidth != gstate->sent.stroke.linewidth)
		{
			if (p->chain->op_w)
				p->chain->op_w(ctx, p->chain, gstate->pending.stroke.linewidth);
		}
		if (gstate->pending.stroke.miterlimit != gstate->sent.stroke.miterlimit)
		{
			if (p->chain->op_M)
				p->chain->op_M(ctx, p->chain, gstate->pending.stroke.miterlimit);
		}
		gstate->sent.stroke = gstate->pending.stroke;
	}

	if (flush & FLUSH_TEXT)
	{
		if (p->BT_pending)
		{
			if (p->chain->op_BT)
				p->chain->op_BT(ctx, p->chain);
			p->BT_pending = 0;
		}
		if (gstate->pending.text.char_space != gstate->sent.text.char_space)
		{
			if (p->chain->op_Tc)
				p->chain->op_Tc(ctx, p->chain, gstate->pending.text.char_space);
		}
		if (gstate->pending.text.word_space != gstate->sent.text.word_space)
		{
			if (p->chain->op_Tw)
				p->chain->op_Tw(ctx, p->chain, gstate->pending.text.word_space);
		}
		if (gstate->pending.text.scale != gstate->sent.text.scale)
		{
			/* The value of scale in the gstate is divided by 100 from what is written in the file */
			if (p->chain->op_Tz)
				p->chain->op_Tz(ctx, p->chain, gstate->pending.text.scale*100);
		}
		if (gstate->pending.text.leading != gstate->sent.text.leading)
		{
			if (p->chain->op_TL)
				p->chain->op_TL(ctx, p->chain, gstate->pending.text.leading);
		}
		if (gstate->pending.text.font != gstate->sent.text.font ||
			gstate->pending.text.size != gstate->sent.text.size)
		{
			if (p->chain->op_Tf)
				p->chain->op_Tf(ctx, p->chain, p->font_name, gstate->pending.text.font, gstate->pending.text.size);
		}
		if (gstate->pending.text.render != gstate->sent.text.render)
		{
			if (p->chain->op_Tr)
				p->chain->op_Tr(ctx, p->chain, gstate->pending.text.render);
		}
		if (gstate->pending.text.rise != gstate->sent.text.rise)
		{
			if (p->chain->op_Ts)
				p->chain->op_Ts(ctx, p->chain, gstate->pending.text.rise);
		}
		pdf_drop_font(ctx, gstate->sent.text.font);
		gstate->sent.text = gstate->pending.text;
		gstate->sent.text.font = pdf_keep_font(ctx, gstate->pending.text.font);
		if (p->Tm_pending != 0)
		{
			if (p->chain->op_Tm)
				p->chain->op_Tm(ctx, p->chain, p->tos.tlm.a, p->tos.tlm.b, p->tos.tlm.c, p->tos.tlm.d, p->tos.tlm.e, p->tos.tlm.f);
			p->Tm_pending = 0;
		}
	}
}
