void dcn20_calculate_dlg_params(
		struct dc *dc, struct dc_state *context,
		display_e2e_pipe_params_st *pipes,
		int pipe_cnt,
		int vlevel)
{
	int i, j, pipe_idx, pipe_idx_unsplit;
	bool visited[MAX_PIPES] = { 0 };

	/* Writeback MCIF_WB arbitration parameters */
	dc->res_pool->funcs->set_mcif_arb_params(dc, context, pipes, pipe_cnt);

	context->bw_ctx.bw.dcn.clk.dispclk_khz = context->bw_ctx.dml.vba.DISPCLK * 1000;
	context->bw_ctx.bw.dcn.clk.dcfclk_khz = context->bw_ctx.dml.vba.DCFCLK * 1000;
	context->bw_ctx.bw.dcn.clk.socclk_khz = context->bw_ctx.dml.vba.SOCCLK * 1000;
	context->bw_ctx.bw.dcn.clk.dramclk_khz = context->bw_ctx.dml.vba.DRAMSpeed * 1000 / 16;
	context->bw_ctx.bw.dcn.clk.dcfclk_deep_sleep_khz = context->bw_ctx.dml.vba.DCFCLKDeepSleep * 1000;
	context->bw_ctx.bw.dcn.clk.fclk_khz = 0;
	context->bw_ctx.bw.dcn.clk.p_state_change_support =
		context->bw_ctx.dml.vba.DRAMClockChangeSupport[vlevel][context->bw_ctx.dml.vba.maxMpcComb]
							!= dm_dram_clock_change_unsupported;
	context->bw_ctx.bw.dcn.clk.dppclk_khz = 0;

	/*
	 * An artifact of dml pipe split/odm is that pipes get merged back together for
	 * calculation. Therefore we need to only extract for first pipe in ascending index order
	 * and copy into the other split half.
	 */
	for (i = 0, pipe_idx = 0, pipe_idx_unsplit = 0; i < dc->res_pool->pipe_count; i++) {
		if (!context->res_ctx.pipe_ctx[i].stream)
			continue;

		if (!visited[pipe_idx]) {
			display_pipe_source_params_st *src = &pipes[pipe_idx_unsplit].pipe.src;
			display_pipe_dest_params_st *dst = &pipes[pipe_idx_unsplit].pipe.dest;

			dst->vstartup_start = context->bw_ctx.dml.vba.VStartup[pipe_idx_unsplit];
			dst->vupdate_offset = context->bw_ctx.dml.vba.VUpdateOffsetPix[pipe_idx_unsplit];
			dst->vupdate_width = context->bw_ctx.dml.vba.VUpdateWidthPix[pipe_idx_unsplit];
			dst->vready_offset = context->bw_ctx.dml.vba.VReadyOffsetPix[pipe_idx_unsplit];
			/*
			 * j iterates inside pipes array, unlike i which iterates inside
			 * pipe_ctx array
			 */
			if (src->is_hsplit)
				for (j = pipe_idx + 1; j < pipe_cnt; j++) {
					display_pipe_source_params_st *src_j = &pipes[j].pipe.src;
					display_pipe_dest_params_st *dst_j = &pipes[j].pipe.dest;

					if (src_j->is_hsplit && !visited[j]
							&& src->hsplit_grp == src_j->hsplit_grp) {
						dst_j->vstartup_start = context->bw_ctx.dml.vba.VStartup[pipe_idx_unsplit];
						dst_j->vupdate_offset = context->bw_ctx.dml.vba.VUpdateOffsetPix[pipe_idx_unsplit];
						dst_j->vupdate_width = context->bw_ctx.dml.vba.VUpdateWidthPix[pipe_idx_unsplit];
						dst_j->vready_offset = context->bw_ctx.dml.vba.VReadyOffsetPix[pipe_idx_unsplit];
						visited[j] = true;
					}
				}
			visited[pipe_idx] = true;
			pipe_idx_unsplit++;
		}
		pipe_idx++;
	}

	for (i = 0, pipe_idx = 0; i < dc->res_pool->pipe_count; i++) {
		if (!context->res_ctx.pipe_ctx[i].stream)
			continue;
		if (context->bw_ctx.bw.dcn.clk.dppclk_khz < pipes[pipe_idx].clks_cfg.dppclk_mhz * 1000)
			context->bw_ctx.bw.dcn.clk.dppclk_khz = pipes[pipe_idx].clks_cfg.dppclk_mhz * 1000;
		context->res_ctx.pipe_ctx[i].plane_res.bw.dppclk_khz =
						pipes[pipe_idx].clks_cfg.dppclk_mhz * 1000;
		ASSERT(visited[pipe_idx]);
		context->res_ctx.pipe_ctx[i].pipe_dlg_param = pipes[pipe_idx].pipe.dest;
		pipe_idx++;
	}
	/*save a original dppclock copy*/
	context->bw_ctx.bw.dcn.clk.bw_dppclk_khz = context->bw_ctx.bw.dcn.clk.dppclk_khz;
	context->bw_ctx.bw.dcn.clk.bw_dispclk_khz = context->bw_ctx.bw.dcn.clk.dispclk_khz;
	context->bw_ctx.bw.dcn.clk.max_supported_dppclk_khz = context->bw_ctx.dml.soc.clock_limits[vlevel].dppclk_mhz * 1000;
	context->bw_ctx.bw.dcn.clk.max_supported_dispclk_khz = context->bw_ctx.dml.soc.clock_limits[vlevel].dispclk_mhz * 1000;

	for (i = 0, pipe_idx = 0; i < dc->res_pool->pipe_count; i++) {
		bool cstate_en = context->bw_ctx.dml.vba.PrefetchMode[vlevel][context->bw_ctx.dml.vba.maxMpcComb] != 2;

		if (!context->res_ctx.pipe_ctx[i].stream)
			continue;

		context->bw_ctx.dml.funcs.rq_dlg_get_dlg_reg(&context->bw_ctx.dml,
				&context->res_ctx.pipe_ctx[i].dlg_regs,
				&context->res_ctx.pipe_ctx[i].ttu_regs,
				pipes,
				pipe_cnt,
				pipe_idx,
				cstate_en,
				context->bw_ctx.bw.dcn.clk.p_state_change_support,
				false, false, false);

		context->bw_ctx.dml.funcs.rq_dlg_get_rq_reg(&context->bw_ctx.dml,
				&context->res_ctx.pipe_ctx[i].rq_regs,
				pipes[pipe_idx].pipe);
		pipe_idx++;
	}
}