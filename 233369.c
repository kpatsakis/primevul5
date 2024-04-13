bool dcn20_validate_bandwidth(struct dc *dc, struct dc_state *context,
		bool fast_validate)
{
	bool voltage_supported = false;
	bool full_pstate_supported = false;
	bool dummy_pstate_supported = false;
	double p_state_latency_us = context->bw_ctx.dml.soc.dram_clock_change_latency_us;

	if (fast_validate)
		return dcn20_validate_bandwidth_internal(dc, context, true);


	// Best case, we support full UCLK switch latency
	voltage_supported = dcn20_validate_bandwidth_internal(dc, context, false);
	full_pstate_supported = context->bw_ctx.bw.dcn.clk.p_state_change_support;

	if (context->bw_ctx.dml.soc.dummy_pstate_latency_us == 0 ||
		(voltage_supported && full_pstate_supported)) {
		context->bw_ctx.bw.dcn.clk.p_state_change_support = true;
		goto restore_dml_state;
	}

	// Fallback: Try to only support G6 temperature read latency
	context->bw_ctx.dml.soc.dram_clock_change_latency_us = context->bw_ctx.dml.soc.dummy_pstate_latency_us;

	voltage_supported = dcn20_validate_bandwidth_internal(dc, context, false);
	dummy_pstate_supported = context->bw_ctx.bw.dcn.clk.p_state_change_support;

	if (voltage_supported && dummy_pstate_supported) {
		context->bw_ctx.bw.dcn.clk.p_state_change_support = false;
		goto restore_dml_state;
	}

	// ERROR: fallback is supposed to always work.
	ASSERT(false);

restore_dml_state:
	memcpy(&context->bw_ctx.dml, &dc->dml, sizeof(struct display_mode_lib));
	context->bw_ctx.dml.soc.dram_clock_change_latency_us = p_state_latency_us;

	return voltage_supported;
}