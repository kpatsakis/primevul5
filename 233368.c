void dcn20_set_mcif_arb_params(
		struct dc *dc,
		struct dc_state *context,
		display_e2e_pipe_params_st *pipes,
		int pipe_cnt)
{
	enum mmhubbub_wbif_mode wbif_mode;
	struct mcif_arb_params *wb_arb_params;
	int i, j, k, dwb_pipe;

	/* Writeback MCIF_WB arbitration parameters */
	dwb_pipe = 0;
	for (i = 0; i < dc->res_pool->pipe_count; i++) {

		if (!context->res_ctx.pipe_ctx[i].stream)
			continue;

		for (j = 0; j < MAX_DWB_PIPES; j++) {
			if (context->res_ctx.pipe_ctx[i].stream->writeback_info[j].wb_enabled == false)
				continue;

			//wb_arb_params = &context->res_ctx.pipe_ctx[i].stream->writeback_info[j].mcif_arb_params;
			wb_arb_params = &context->bw_ctx.bw.dcn.bw_writeback.mcif_wb_arb[dwb_pipe];

			if (context->res_ctx.pipe_ctx[i].stream->writeback_info[j].dwb_params.out_format == dwb_scaler_mode_yuv420) {
				if (context->res_ctx.pipe_ctx[i].stream->writeback_info[j].dwb_params.output_depth == DWB_OUTPUT_PIXEL_DEPTH_8BPC)
					wbif_mode = PLANAR_420_8BPC;
				else
					wbif_mode = PLANAR_420_10BPC;
			} else
				wbif_mode = PACKED_444;

			for (k = 0; k < sizeof(wb_arb_params->cli_watermark)/sizeof(wb_arb_params->cli_watermark[0]); k++) {
				wb_arb_params->cli_watermark[k] = get_wm_writeback_urgent(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
				wb_arb_params->pstate_watermark[k] = get_wm_writeback_dram_clock_change(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
			}
			wb_arb_params->time_per_pixel = 16.0 / context->res_ctx.pipe_ctx[i].stream->phy_pix_clk; /* 4 bit fraction, ms */
			wb_arb_params->slice_lines = 32;
			wb_arb_params->arbitration_slice = 2;
			wb_arb_params->max_scaled_time = dcn20_calc_max_scaled_time(wb_arb_params->time_per_pixel,
				wbif_mode,
				wb_arb_params->cli_watermark[0]); /* assume 4 watermark sets have the same value */

			dwb_pipe++;

			if (dwb_pipe >= MAX_DWB_PIPES)
				return;
		}
		if (dwb_pipe >= MAX_DWB_PIPES)
			return;
	}
}