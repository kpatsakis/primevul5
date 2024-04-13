static s64 ttml_get_timestamp(GF_TXTIn *ctx, char *value)
{
	u32 h, m, s, ms, f, sf;
	s64 ts = -1;
	u32 len = (u32) strlen(value);

	//tick metrick - cannot be fractional
	if (len && (value[len-1]=='t')) {
		value[len-1] = 0;
		ts = (s64) (atoi(value) * 1000);
		value[len-1] = 't';
		if (ctx->tick_rate)
			ts /= ctx->tick_rate;
	}
	//hours metric, can be fractional
	else if (len && (value[len-1]=='h')) {
		value[len-1] = 0;
		ts = (s64) (atof(value) * 1000 * 3600);
		value[len-1] = 'h';
	}
	//minutes metric, can be fractional
	else if (len && (value[len-1]=='m')) {
		value[len-1] = 0;
		ts = (s64) (atof(value) * 1000 * 60);
		value[len-1] = 'm';
	}
	else if (len && (value[len-1]=='s')) {
		//milliseconds metric, can be fractional but we work at 1ms clock resolution anyway
		if ((len > 1) && (value[len-2]=='m')) {
			value[len-2] = 0;
			ts = (s64) (atof(value));
			value[len-2] = 'm';
		}
		//seconds metric, can be fractional
		else {
			value[len-1] = 0;
			ts = (s64) (atof(value) * 1000);
			value[len-1] = 's';
		}
	}
	//frames metric, can be fractional
	else if (len && (value[len-1]=='f')) {
		f = sf = 0;
		value[len-1] = 0;
		if (sscanf(value, "%u.%u", &f, &sf) != 2) {
			sscanf(value, "%u", &f);
			sf = 0;
		}
		value[len-1] = 'f';

		if (!ctx->ttml_fps_num) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TTML EBU-TTD] time indicates frames but no frame rate set, assuming 25 FPS\n"));
			ctx->ttml_fps_num = 25;
			ctx->ttml_fps_den = 1;
		}
		if (sf && !ctx->ttml_sfps) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TTML EBU-TTD] time indicates subframes but no subFrameRate set, assuming 1\n"));
			ctx->ttml_sfps = 1;
		}
		ts = ((s64) 1000 * f * ctx->ttml_fps_den) / ctx->ttml_fps_num;
		if (sf)
			ts += ((s64) 1000 * sf * ctx->ttml_fps_den / ctx->ttml_sfps) / ctx->ttml_fps_num;
	}
	else if (sscanf(value, "%u:%u:%u.%u", &h, &m, &s, &ms) == 4) {
		ts = (h*3600 + m*60+s)*1000+ms;
	}
	else if (sscanf(value, "%u:%u:%u:%u.%u", &h, &m, &s, &f, &sf) == 5) {
		ts = (h*3600 + m*60+s)*1000;
		if (!ctx->ttml_fps_num) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TTML EBU-TTD] time indicates frames but no frame rate set, assuming 25 FPS\n"));
			ctx->ttml_fps_num = 25;
			ctx->ttml_fps_den = 1;
		}
		if (!ctx->ttml_sfps) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TTML EBU-TTD] time indicates subframes but no subFrameRate set, assuming 1\n"));
			ctx->ttml_sfps = 1;
		}
		ts += ((s64) 1000 * f * ctx->ttml_fps_den) / ctx->ttml_fps_num;
		ts += ((s64) 1000 * sf * ctx->ttml_fps_den / ctx->ttml_sfps) / ctx->ttml_fps_num;
	}
	else if (sscanf(value, "%u:%u:%u:%u", &h, &m, &s, &f) == 4) {
		ts = (h*3600 + m*60+s)*1000;
		if (!ctx->ttml_fps_num) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("[TTML EBU-TTD] time indicates frames but no frame rate set, assuming 25 FPS\n"));
			ctx->ttml_fps_num = 25;
			ctx->ttml_fps_den = 1;
		}
		ts += ((s64) 1000 * f * ctx->ttml_fps_den) / ctx->ttml_fps_num;
	}
	else if (sscanf(value, "%u:%u:%u", &h, &m, &s) == 3) {
		ts = (h*3600 + m*60+s)*1000;
	}
	return ts;
}