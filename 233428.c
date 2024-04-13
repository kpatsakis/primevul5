int dcn20_populate_dml_pipes_from_context(
		struct dc *dc, struct resource_context *res_ctx, display_e2e_pipe_params_st *pipes)
{
	int pipe_cnt, i;
	bool synchronized_vblank = true;

	for (i = 0, pipe_cnt = -1; i < dc->res_pool->pipe_count; i++) {
		if (!res_ctx->pipe_ctx[i].stream)
			continue;

		if (pipe_cnt < 0) {
			pipe_cnt = i;
			continue;
		}
		if (!resource_are_streams_timing_synchronizable(
				res_ctx->pipe_ctx[pipe_cnt].stream,
				res_ctx->pipe_ctx[i].stream)) {
			synchronized_vblank = false;
			break;
		}
	}

	for (i = 0, pipe_cnt = 0; i < dc->res_pool->pipe_count; i++) {
		struct dc_crtc_timing *timing = &res_ctx->pipe_ctx[i].stream->timing;
		int output_bpc;

		if (!res_ctx->pipe_ctx[i].stream)
			continue;
		/* todo:
		pipes[pipe_cnt].pipe.src.dynamic_metadata_enable = 0;
		pipes[pipe_cnt].pipe.src.dcc = 0;
		pipes[pipe_cnt].pipe.src.vm = 0;*/

#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
		pipes[pipe_cnt].dout.dsc_enable = res_ctx->pipe_ctx[i].stream->timing.flags.DSC;
		/* todo: rotation?*/
		pipes[pipe_cnt].dout.dsc_slices = res_ctx->pipe_ctx[i].stream->timing.dsc_cfg.num_slices_h;
#endif
		if (res_ctx->pipe_ctx[i].stream->use_dynamic_meta) {
			pipes[pipe_cnt].pipe.src.dynamic_metadata_enable = true;
			/* 1/2 vblank */
			pipes[pipe_cnt].pipe.src.dynamic_metadata_lines_before_active =
				(timing->v_total - timing->v_addressable
					- timing->v_border_top - timing->v_border_bottom) / 2;
			/* 36 bytes dp, 32 hdmi */
			pipes[pipe_cnt].pipe.src.dynamic_metadata_xmit_bytes =
				dc_is_dp_signal(res_ctx->pipe_ctx[i].stream->signal) ? 36 : 32;
		}
		pipes[pipe_cnt].pipe.src.dcc = false;
		pipes[pipe_cnt].pipe.src.dcc_rate = 1;
		pipes[pipe_cnt].pipe.dest.synchronized_vblank_all_planes = synchronized_vblank;
		pipes[pipe_cnt].pipe.dest.hblank_start = timing->h_total - timing->h_front_porch;
		pipes[pipe_cnt].pipe.dest.hblank_end = pipes[pipe_cnt].pipe.dest.hblank_start
				- timing->h_addressable
				- timing->h_border_left
				- timing->h_border_right;
		pipes[pipe_cnt].pipe.dest.vblank_start = timing->v_total - timing->v_front_porch;
		pipes[pipe_cnt].pipe.dest.vblank_end = pipes[pipe_cnt].pipe.dest.vblank_start
				- timing->v_addressable
				- timing->v_border_top
				- timing->v_border_bottom;
		pipes[pipe_cnt].pipe.dest.htotal = timing->h_total;
		pipes[pipe_cnt].pipe.dest.vtotal = timing->v_total;
		pipes[pipe_cnt].pipe.dest.hactive = timing->h_addressable;
		pipes[pipe_cnt].pipe.dest.vactive = timing->v_addressable;
		pipes[pipe_cnt].pipe.dest.interlaced = timing->flags.INTERLACE;
		pipes[pipe_cnt].pipe.dest.pixel_rate_mhz = timing->pix_clk_100hz/10000.0;
		if (timing->timing_3d_format == TIMING_3D_FORMAT_HW_FRAME_PACKING)
			pipes[pipe_cnt].pipe.dest.pixel_rate_mhz *= 2;
		pipes[pipe_cnt].pipe.dest.otg_inst = res_ctx->pipe_ctx[i].stream_res.tg->inst;
		pipes[pipe_cnt].dout.dp_lanes = 4;
		pipes[pipe_cnt].pipe.dest.vtotal_min = res_ctx->pipe_ctx[i].stream->adjust.v_total_min;
		pipes[pipe_cnt].pipe.dest.vtotal_max = res_ctx->pipe_ctx[i].stream->adjust.v_total_max;
		pipes[pipe_cnt].pipe.dest.odm_combine = res_ctx->pipe_ctx[i].prev_odm_pipe
							|| res_ctx->pipe_ctx[i].next_odm_pipe;
		pipes[pipe_cnt].pipe.src.hsplit_grp = res_ctx->pipe_ctx[i].pipe_idx;
		if (res_ctx->pipe_ctx[i].top_pipe && res_ctx->pipe_ctx[i].top_pipe->plane_state
				== res_ctx->pipe_ctx[i].plane_state)
			pipes[pipe_cnt].pipe.src.hsplit_grp = res_ctx->pipe_ctx[i].top_pipe->pipe_idx;
		else if (res_ctx->pipe_ctx[i].prev_odm_pipe) {
			struct pipe_ctx *first_pipe = res_ctx->pipe_ctx[i].prev_odm_pipe;

			while (first_pipe->prev_odm_pipe)
				first_pipe = first_pipe->prev_odm_pipe;
			pipes[pipe_cnt].pipe.src.hsplit_grp = first_pipe->pipe_idx;
		}

		switch (res_ctx->pipe_ctx[i].stream->signal) {
		case SIGNAL_TYPE_DISPLAY_PORT_MST:
		case SIGNAL_TYPE_DISPLAY_PORT:
			pipes[pipe_cnt].dout.output_type = dm_dp;
			break;
		case SIGNAL_TYPE_EDP:
			pipes[pipe_cnt].dout.output_type = dm_edp;
			break;
		case SIGNAL_TYPE_HDMI_TYPE_A:
		case SIGNAL_TYPE_DVI_SINGLE_LINK:
		case SIGNAL_TYPE_DVI_DUAL_LINK:
			pipes[pipe_cnt].dout.output_type = dm_hdmi;
			break;
		default:
			/* In case there is no signal, set dp with 4 lanes to allow max config */
			pipes[pipe_cnt].dout.output_type = dm_dp;
			pipes[pipe_cnt].dout.dp_lanes = 4;
		}

		switch (res_ctx->pipe_ctx[i].stream->timing.display_color_depth) {
		case COLOR_DEPTH_666:
			output_bpc = 6;
			break;
		case COLOR_DEPTH_888:
			output_bpc = 8;
			break;
		case COLOR_DEPTH_101010:
			output_bpc = 10;
			break;
		case COLOR_DEPTH_121212:
			output_bpc = 12;
			break;
		case COLOR_DEPTH_141414:
			output_bpc = 14;
			break;
		case COLOR_DEPTH_161616:
			output_bpc = 16;
			break;
#ifdef CONFIG_DRM_AMD_DC_DCN2_0
		case COLOR_DEPTH_999:
			output_bpc = 9;
			break;
		case COLOR_DEPTH_111111:
			output_bpc = 11;
			break;
#endif
		default:
			output_bpc = 8;
			break;
		}

		switch (res_ctx->pipe_ctx[i].stream->timing.pixel_encoding) {
		case PIXEL_ENCODING_RGB:
		case PIXEL_ENCODING_YCBCR444:
			pipes[pipe_cnt].dout.output_format = dm_444;
			pipes[pipe_cnt].dout.output_bpp = output_bpc * 3;
			break;
		case PIXEL_ENCODING_YCBCR420:
			pipes[pipe_cnt].dout.output_format = dm_420;
			pipes[pipe_cnt].dout.output_bpp = (output_bpc * 3) / 2;
			break;
		case PIXEL_ENCODING_YCBCR422:
			if (true) /* todo */
				pipes[pipe_cnt].dout.output_format = dm_s422;
			else
				pipes[pipe_cnt].dout.output_format = dm_n422;
			pipes[pipe_cnt].dout.output_bpp = output_bpc * 2;
			break;
		default:
			pipes[pipe_cnt].dout.output_format = dm_444;
			pipes[pipe_cnt].dout.output_bpp = output_bpc * 3;
		}

		/* todo: default max for now, until there is logic reflecting this in dc*/
		pipes[pipe_cnt].dout.output_bpc = 12;
		/*
		 * Use max cursor settings for calculations to minimize
		 * bw calculations due to cursor on/off
		 */
		pipes[pipe_cnt].pipe.src.num_cursors = 2;
		pipes[pipe_cnt].pipe.src.cur0_src_width = 256;
		pipes[pipe_cnt].pipe.src.cur0_bpp = dm_cur_32bit;
		pipes[pipe_cnt].pipe.src.cur1_src_width = 256;
		pipes[pipe_cnt].pipe.src.cur1_bpp = dm_cur_32bit;

		if (!res_ctx->pipe_ctx[i].plane_state) {
			pipes[pipe_cnt].pipe.src.source_scan = dm_horz;
			pipes[pipe_cnt].pipe.src.sw_mode = dm_sw_linear;
			pipes[pipe_cnt].pipe.src.macro_tile_size = dm_64k_tile;
			pipes[pipe_cnt].pipe.src.viewport_width = timing->h_addressable;
			if (pipes[pipe_cnt].pipe.src.viewport_width > 1920)
				pipes[pipe_cnt].pipe.src.viewport_width = 1920;
			pipes[pipe_cnt].pipe.src.viewport_height = timing->v_addressable;
			if (pipes[pipe_cnt].pipe.src.viewport_height > 1080)
				pipes[pipe_cnt].pipe.src.viewport_height = 1080;
			pipes[pipe_cnt].pipe.src.data_pitch = ((pipes[pipe_cnt].pipe.src.viewport_width + 63) / 64) * 64; /* linear sw only */
			pipes[pipe_cnt].pipe.src.source_format = dm_444_32;
			pipes[pipe_cnt].pipe.dest.recout_width = pipes[pipe_cnt].pipe.src.viewport_width; /*vp_width/hratio*/
			pipes[pipe_cnt].pipe.dest.recout_height = pipes[pipe_cnt].pipe.src.viewport_height; /*vp_height/vratio*/
			pipes[pipe_cnt].pipe.dest.full_recout_width = pipes[pipe_cnt].pipe.dest.recout_width;  /*when is_hsplit != 1*/
			pipes[pipe_cnt].pipe.dest.full_recout_height = pipes[pipe_cnt].pipe.dest.recout_height; /*when is_hsplit != 1*/
			pipes[pipe_cnt].pipe.scale_ratio_depth.lb_depth = dm_lb_16;
			pipes[pipe_cnt].pipe.scale_ratio_depth.hscl_ratio = 1.0;
			pipes[pipe_cnt].pipe.scale_ratio_depth.vscl_ratio = 1.0;
			pipes[pipe_cnt].pipe.scale_ratio_depth.scl_enable = 0; /*Lb only or Full scl*/
			pipes[pipe_cnt].pipe.scale_taps.htaps = 1;
			pipes[pipe_cnt].pipe.scale_taps.vtaps = 1;
			pipes[pipe_cnt].pipe.src.is_hsplit = 0;
			pipes[pipe_cnt].pipe.dest.odm_combine = 0;
			pipes[pipe_cnt].pipe.dest.vtotal_min = timing->v_total;
			pipes[pipe_cnt].pipe.dest.vtotal_max = timing->v_total;
		} else {
			struct dc_plane_state *pln = res_ctx->pipe_ctx[i].plane_state;
			struct scaler_data *scl = &res_ctx->pipe_ctx[i].plane_res.scl_data;

			pipes[pipe_cnt].pipe.src.immediate_flip = pln->flip_immediate;
			pipes[pipe_cnt].pipe.src.is_hsplit = (res_ctx->pipe_ctx[i].bottom_pipe
					&& res_ctx->pipe_ctx[i].bottom_pipe->plane_state == pln)
					|| (res_ctx->pipe_ctx[i].top_pipe
					&& res_ctx->pipe_ctx[i].top_pipe->plane_state == pln);
			pipes[pipe_cnt].pipe.src.source_scan = pln->rotation == ROTATION_ANGLE_90
					|| pln->rotation == ROTATION_ANGLE_270 ? dm_vert : dm_horz;
			pipes[pipe_cnt].pipe.src.viewport_y_y = scl->viewport.y;
			pipes[pipe_cnt].pipe.src.viewport_y_c = scl->viewport_c.y;
			pipes[pipe_cnt].pipe.src.viewport_width = scl->viewport.width;
			pipes[pipe_cnt].pipe.src.viewport_width_c = scl->viewport_c.width;
			pipes[pipe_cnt].pipe.src.viewport_height = scl->viewport.height;
			pipes[pipe_cnt].pipe.src.viewport_height_c = scl->viewport_c.height;
			if (pln->format >= SURFACE_PIXEL_FORMAT_VIDEO_BEGIN) {
				pipes[pipe_cnt].pipe.src.data_pitch = pln->plane_size.surface_pitch;
				pipes[pipe_cnt].pipe.src.data_pitch_c = pln->plane_size.chroma_pitch;
				pipes[pipe_cnt].pipe.src.meta_pitch = pln->dcc.meta_pitch;
				pipes[pipe_cnt].pipe.src.meta_pitch_c = pln->dcc.meta_pitch_c;
			} else {
				pipes[pipe_cnt].pipe.src.data_pitch = pln->plane_size.surface_pitch;
				pipes[pipe_cnt].pipe.src.meta_pitch = pln->dcc.meta_pitch;
			}
			pipes[pipe_cnt].pipe.src.dcc = pln->dcc.enable;
			pipes[pipe_cnt].pipe.dest.recout_width = scl->recout.width;
			pipes[pipe_cnt].pipe.dest.recout_height = scl->recout.height;
			pipes[pipe_cnt].pipe.dest.full_recout_width = scl->recout.width;
			pipes[pipe_cnt].pipe.dest.full_recout_height = scl->recout.height;
			if (res_ctx->pipe_ctx[i].bottom_pipe && res_ctx->pipe_ctx[i].bottom_pipe->plane_state == pln) {
				pipes[pipe_cnt].pipe.dest.full_recout_width +=
						res_ctx->pipe_ctx[i].bottom_pipe->plane_res.scl_data.recout.width;
				pipes[pipe_cnt].pipe.dest.full_recout_height +=
						res_ctx->pipe_ctx[i].bottom_pipe->plane_res.scl_data.recout.height;
			} else if (res_ctx->pipe_ctx[i].top_pipe && res_ctx->pipe_ctx[i].top_pipe->plane_state == pln) {
				pipes[pipe_cnt].pipe.dest.full_recout_width +=
						res_ctx->pipe_ctx[i].top_pipe->plane_res.scl_data.recout.width;
				pipes[pipe_cnt].pipe.dest.full_recout_height +=
						res_ctx->pipe_ctx[i].top_pipe->plane_res.scl_data.recout.height;
			}

			pipes[pipe_cnt].pipe.scale_ratio_depth.lb_depth = dm_lb_16;
			pipes[pipe_cnt].pipe.scale_ratio_depth.hscl_ratio = (double) scl->ratios.horz.value / (1ULL<<32);
			pipes[pipe_cnt].pipe.scale_ratio_depth.hscl_ratio_c = (double) scl->ratios.horz_c.value / (1ULL<<32);
			pipes[pipe_cnt].pipe.scale_ratio_depth.vscl_ratio = (double) scl->ratios.vert.value / (1ULL<<32);
			pipes[pipe_cnt].pipe.scale_ratio_depth.vscl_ratio_c = (double) scl->ratios.vert_c.value / (1ULL<<32);
			pipes[pipe_cnt].pipe.scale_ratio_depth.scl_enable =
					scl->ratios.vert.value != dc_fixpt_one.value
					|| scl->ratios.horz.value != dc_fixpt_one.value
					|| scl->ratios.vert_c.value != dc_fixpt_one.value
					|| scl->ratios.horz_c.value != dc_fixpt_one.value /*Lb only or Full scl*/
					|| dc->debug.always_scale; /*support always scale*/
			pipes[pipe_cnt].pipe.scale_taps.htaps = scl->taps.h_taps;
			pipes[pipe_cnt].pipe.scale_taps.htaps_c = scl->taps.h_taps_c;
			pipes[pipe_cnt].pipe.scale_taps.vtaps = scl->taps.v_taps;
			pipes[pipe_cnt].pipe.scale_taps.vtaps_c = scl->taps.v_taps_c;

			pipes[pipe_cnt].pipe.src.macro_tile_size =
					swizzle_mode_to_macro_tile_size(pln->tiling_info.gfx9.swizzle);
			swizzle_to_dml_params(pln->tiling_info.gfx9.swizzle,
					&pipes[pipe_cnt].pipe.src.sw_mode);

			switch (pln->format) {
			case SURFACE_PIXEL_FORMAT_VIDEO_420_YCbCr:
			case SURFACE_PIXEL_FORMAT_VIDEO_420_YCrCb:
				pipes[pipe_cnt].pipe.src.source_format = dm_420_8;
				break;
			case SURFACE_PIXEL_FORMAT_VIDEO_420_10bpc_YCbCr:
			case SURFACE_PIXEL_FORMAT_VIDEO_420_10bpc_YCrCb:
				pipes[pipe_cnt].pipe.src.source_format = dm_420_10;
				break;
			case SURFACE_PIXEL_FORMAT_GRPH_ARGB16161616:
			case SURFACE_PIXEL_FORMAT_GRPH_ARGB16161616F:
			case SURFACE_PIXEL_FORMAT_GRPH_ABGR16161616F:
				pipes[pipe_cnt].pipe.src.source_format = dm_444_64;
				break;
			case SURFACE_PIXEL_FORMAT_GRPH_ARGB1555:
			case SURFACE_PIXEL_FORMAT_GRPH_RGB565:
				pipes[pipe_cnt].pipe.src.source_format = dm_444_16;
				break;
			case SURFACE_PIXEL_FORMAT_GRPH_PALETA_256_COLORS:
				pipes[pipe_cnt].pipe.src.source_format = dm_444_8;
				break;
			default:
				pipes[pipe_cnt].pipe.src.source_format = dm_444_32;
				break;
			}
		}

		pipe_cnt++;
	}

	/* populate writeback information */
	dc->res_pool->funcs->populate_dml_writeback_from_context(dc, res_ctx, pipes);

	return pipe_cnt;
}