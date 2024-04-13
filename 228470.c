sdhtml_toc_renderer(struct sd_callbacks *callbacks, struct html_renderopt *options, unsigned int render_flags)
{
	static const struct sd_callbacks cb_default = {
		NULL,
		NULL,
		NULL,
		toc_header,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		rndr_footnotes,
		rndr_footnote_def,

		NULL,
		rndr_codespan,
		rndr_double_emphasis,
		rndr_emphasis,
		rndr_underline,
		rndr_highlight,
		rndr_quote,
		NULL,
		NULL,
		toc_link,
		NULL,
		rndr_triple_emphasis,
		rndr_strikethrough,
		rndr_superscript,
		rndr_footnote_ref,

		NULL,
		NULL,

		NULL,
		toc_finalize,
	};

	memset(options, 0x0, sizeof(struct html_renderopt));
	options->flags = render_flags;

	memcpy(callbacks, &cb_default, sizeof(struct sd_callbacks));
}