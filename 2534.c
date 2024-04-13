static GF_Err txtin_configure_pid(GF_Filter *filter, GF_FilterPid *pid, Bool is_remove)
{
	GF_Err e;
	const char *src = NULL;
	GF_TXTIn *ctx = gf_filter_get_udta(filter);
	const GF_PropertyValue *prop;

	if (is_remove) {
		ctx->ipid = NULL;
		return GF_OK;
	}

	if (! gf_filter_pid_check_caps(pid))
		return GF_NOT_SUPPORTED;

	//we must have a file path
	prop = gf_filter_pid_get_property(pid, GF_PROP_PID_FILEPATH);
	if (prop && prop->value.string) src = prop->value.string;
	if (!src)
		return GF_NOT_SUPPORTED;

	if (!ctx->ipid) {
		GF_FilterEvent fevt;
		ctx->ipid = pid;

		//we work with full file only, send a play event on source to indicate that
		GF_FEVT_INIT(fevt, GF_FEVT_PLAY, pid);
		fevt.play.start_range = 0;
		fevt.base.on_pid = ctx->ipid;
		fevt.play.full_file_only = GF_TRUE;
		gf_filter_pid_send_event(ctx->ipid, &fevt);
		ctx->file_name = gf_strdup(src);
	} else {
		if (pid != ctx->ipid) {
			return GF_REQUIRES_NEW_INSTANCE;
		}
		if (!strcmp(ctx->file_name, src)) return GF_OK;

		ttxtin_reset(ctx);
		ctx->is_setup = GF_FALSE;
		gf_free(ctx->file_name);
		ctx->file_name = gf_strdup(src);
	}
	//guess type
	e = gf_text_guess_format(ctx->file_name, &ctx->fmt);
	if (e) return e;
	if (!ctx->fmt) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TXTLoad] Unknown text format for %s\n", ctx->file_name));
		return GF_NOT_SUPPORTED;
	}

	if (ctx->webvtt && (ctx->fmt == GF_TXTIN_MODE_SRT))
		ctx->fmt = GF_TXTIN_MODE_WEBVTT;

	switch (ctx->fmt) {
	case GF_TXTIN_MODE_SRT:
		ctx->text_process = txtin_process_srt;
		break;
#ifndef GPAC_DISABLE_VTT
	case GF_TXTIN_MODE_WEBVTT:
		ctx->text_process = txtin_process_webvtt;
		break;
#endif
	case GF_TXTIN_MODE_TTXT:
		ctx->text_process = txtin_process_ttxt;
		break;
	case GF_TXTIN_MODE_TEXML:
		ctx->text_process = txtin_process_texml;
		break;
	case GF_TXTIN_MODE_SUB:
		ctx->text_process = gf_text_process_sub;
		break;
	case GF_TXTIN_MODE_TTML:
		ctx->text_process = gf_text_process_ttml;
		break;
#ifndef GPAC_DISABLE_SWF_IMPORT
	case GF_TXTIN_MODE_SWF_SVG:
		ctx->text_process = gf_text_process_swf;
		break;
#endif
	default:
		return GF_BAD_PARAM;
	}

	return GF_OK;
}