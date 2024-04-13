void dcn20_calculate_wm(
		struct dc *dc, struct dc_state *context,
		display_e2e_pipe_params_st *pipes,
		int *out_pipe_cnt,
		int *pipe_split_from,
		int vlevel)
{
	int pipe_cnt, i, pipe_idx;

	for (i = 0, pipe_idx = 0, pipe_cnt = 0; i < dc->res_pool->pipe_count; i++) {
		if (!context->res_ctx.pipe_ctx[i].stream)
			continue;

		pipes[pipe_cnt].clks_cfg.refclk_mhz = dc->res_pool->ref_clocks.dchub_ref_clock_inKhz / 1000.0;
		pipes[pipe_cnt].clks_cfg.dispclk_mhz = context->bw_ctx.dml.vba.RequiredDISPCLK[vlevel][context->bw_ctx.dml.vba.maxMpcComb];

		if (pipe_split_from[i] < 0) {
			pipes[pipe_cnt].clks_cfg.dppclk_mhz =
					context->bw_ctx.dml.vba.RequiredDPPCLK[vlevel][context->bw_ctx.dml.vba.maxMpcComb][pipe_idx];
			if (context->bw_ctx.dml.vba.BlendingAndTiming[pipe_idx] == pipe_idx)
				pipes[pipe_cnt].pipe.dest.odm_combine =
						context->bw_ctx.dml.vba.ODMCombineEnablePerState[vlevel][pipe_idx];
			else
				pipes[pipe_cnt].pipe.dest.odm_combine = 0;
			pipe_idx++;
		} else {
			pipes[pipe_cnt].clks_cfg.dppclk_mhz =
					context->bw_ctx.dml.vba.RequiredDPPCLK[vlevel][context->bw_ctx.dml.vba.maxMpcComb][pipe_split_from[i]];
			if (context->bw_ctx.dml.vba.BlendingAndTiming[pipe_split_from[i]] == pipe_split_from[i])
				pipes[pipe_cnt].pipe.dest.odm_combine =
						context->bw_ctx.dml.vba.ODMCombineEnablePerState[vlevel][pipe_split_from[i]];
			else
				pipes[pipe_cnt].pipe.dest.odm_combine = 0;
		}

		if (dc->config.forced_clocks) {
			pipes[pipe_cnt].clks_cfg.dispclk_mhz = context->bw_ctx.dml.soc.clock_limits[0].dispclk_mhz;
			pipes[pipe_cnt].clks_cfg.dppclk_mhz = context->bw_ctx.dml.soc.clock_limits[0].dppclk_mhz;
		}
		if (dc->debug.min_disp_clk_khz > pipes[pipe_cnt].clks_cfg.dispclk_mhz * 1000)
			pipes[pipe_cnt].clks_cfg.dispclk_mhz = dc->debug.min_disp_clk_khz / 1000.0;
		if (dc->debug.min_dpp_clk_khz > pipes[pipe_cnt].clks_cfg.dppclk_mhz * 1000)
			pipes[pipe_cnt].clks_cfg.dppclk_mhz = dc->debug.min_dpp_clk_khz / 1000.0;

		pipe_cnt++;
	}

	if (pipe_cnt != pipe_idx) {
		if (dc->res_pool->funcs->populate_dml_pipes)
			pipe_cnt = dc->res_pool->funcs->populate_dml_pipes(dc,
				&context->res_ctx, pipes);
		else
			pipe_cnt = dcn20_populate_dml_pipes_from_context(dc,
				&context->res_ctx, pipes);
	}

	*out_pipe_cnt = pipe_cnt;

	pipes[0].clks_cfg.voltage = vlevel;
	pipes[0].clks_cfg.dcfclk_mhz = context->bw_ctx.dml.soc.clock_limits[vlevel].dcfclk_mhz;
	pipes[0].clks_cfg.socclk_mhz = context->bw_ctx.dml.soc.clock_limits[vlevel].socclk_mhz;

	/* only pipe 0 is read for voltage and dcf/soc clocks */
	if (vlevel < 1) {
		pipes[0].clks_cfg.voltage = 1;
		pipes[0].clks_cfg.dcfclk_mhz = context->bw_ctx.dml.soc.clock_limits[1].dcfclk_mhz;
		pipes[0].clks_cfg.socclk_mhz = context->bw_ctx.dml.soc.clock_limits[1].socclk_mhz;
	}
	context->bw_ctx.bw.dcn.watermarks.b.urgent_ns = get_wm_urgent(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.b.cstate_pstate.cstate_enter_plus_exit_ns = get_wm_stutter_enter_exit(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.b.cstate_pstate.cstate_exit_ns = get_wm_stutter_exit(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.b.cstate_pstate.pstate_change_ns = get_wm_dram_clock_change(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.b.pte_meta_urgent_ns = get_wm_memory_trip(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;

	if (vlevel < 2) {
		pipes[0].clks_cfg.voltage = 2;
		pipes[0].clks_cfg.dcfclk_mhz = context->bw_ctx.dml.soc.clock_limits[2].dcfclk_mhz;
		pipes[0].clks_cfg.socclk_mhz = context->bw_ctx.dml.soc.clock_limits[2].socclk_mhz;
	}
	context->bw_ctx.bw.dcn.watermarks.c.urgent_ns = get_wm_urgent(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.c.cstate_pstate.cstate_enter_plus_exit_ns = get_wm_stutter_enter_exit(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.c.cstate_pstate.cstate_exit_ns = get_wm_stutter_exit(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.c.cstate_pstate.pstate_change_ns = get_wm_dram_clock_change(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.c.pte_meta_urgent_ns = get_wm_memory_trip(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;

	if (vlevel < 3) {
		pipes[0].clks_cfg.voltage = 3;
		pipes[0].clks_cfg.dcfclk_mhz = context->bw_ctx.dml.soc.clock_limits[2].dcfclk_mhz;
		pipes[0].clks_cfg.socclk_mhz = context->bw_ctx.dml.soc.clock_limits[2].socclk_mhz;
	}
	context->bw_ctx.bw.dcn.watermarks.d.urgent_ns = get_wm_urgent(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.d.cstate_pstate.cstate_enter_plus_exit_ns = get_wm_stutter_enter_exit(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.d.cstate_pstate.cstate_exit_ns = get_wm_stutter_exit(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.d.cstate_pstate.pstate_change_ns = get_wm_dram_clock_change(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.d.pte_meta_urgent_ns = get_wm_memory_trip(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;

	pipes[0].clks_cfg.voltage = vlevel;
	pipes[0].clks_cfg.dcfclk_mhz = context->bw_ctx.dml.soc.clock_limits[vlevel].dcfclk_mhz;
	pipes[0].clks_cfg.socclk_mhz = context->bw_ctx.dml.soc.clock_limits[vlevel].socclk_mhz;
	context->bw_ctx.bw.dcn.watermarks.a.urgent_ns = get_wm_urgent(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.a.cstate_pstate.cstate_enter_plus_exit_ns = get_wm_stutter_enter_exit(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.a.cstate_pstate.cstate_exit_ns = get_wm_stutter_exit(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.a.cstate_pstate.pstate_change_ns = get_wm_dram_clock_change(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
	context->bw_ctx.bw.dcn.watermarks.a.pte_meta_urgent_ns = get_wm_memory_trip(&context->bw_ctx.dml, pipes, pipe_cnt) * 1000;
}