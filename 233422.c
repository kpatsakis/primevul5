static void patch_bounding_box(struct dc *dc, struct _vcs_dpi_soc_bounding_box_st *bb)
{
	kernel_fpu_begin();
	if ((int)(bb->sr_exit_time_us * 1000) != dc->bb_overrides.sr_exit_time_ns
			&& dc->bb_overrides.sr_exit_time_ns) {
		bb->sr_exit_time_us = dc->bb_overrides.sr_exit_time_ns / 1000.0;
	}

	if ((int)(bb->sr_enter_plus_exit_time_us * 1000)
				!= dc->bb_overrides.sr_enter_plus_exit_time_ns
			&& dc->bb_overrides.sr_enter_plus_exit_time_ns) {
		bb->sr_enter_plus_exit_time_us =
				dc->bb_overrides.sr_enter_plus_exit_time_ns / 1000.0;
	}

	if ((int)(bb->urgent_latency_us * 1000) != dc->bb_overrides.urgent_latency_ns
			&& dc->bb_overrides.urgent_latency_ns) {
		bb->urgent_latency_us = dc->bb_overrides.urgent_latency_ns / 1000.0;
	}

	if ((int)(bb->dram_clock_change_latency_us * 1000)
				!= dc->bb_overrides.dram_clock_change_latency_ns
			&& dc->bb_overrides.dram_clock_change_latency_ns) {
		bb->dram_clock_change_latency_us =
				dc->bb_overrides.dram_clock_change_latency_ns / 1000.0;
	}
	kernel_fpu_end();
}