static void update_bounding_box(struct dc *dc, struct _vcs_dpi_soc_bounding_box_st *bb,
		struct pp_smu_nv_clock_table *max_clocks, unsigned int *uclk_states, unsigned int num_states)
{
	struct _vcs_dpi_voltage_scaling_st calculated_states[MAX_CLOCK_LIMIT_STATES] = {0};
	int i;
	int num_calculated_states = 0;
	int min_dcfclk = 0;

	if (num_states == 0)
		return;

	if (dc->bb_overrides.min_dcfclk_mhz > 0)
		min_dcfclk = dc->bb_overrides.min_dcfclk_mhz;
	else
		// Accounting for SOC/DCF relationship, we can go as high as
		// 506Mhz in Vmin.  We need to code 507 since SMU will round down to 506.
		min_dcfclk = 507;

	for (i = 0; i < num_states; i++) {
		int min_fclk_required_by_uclk;
		calculated_states[i].state = i;
		calculated_states[i].dram_speed_mts = uclk_states[i] * 16 / 1000;

		// FCLK:UCLK ratio is 1.08
		min_fclk_required_by_uclk = mul_u64_u32_shr(BIT_ULL(32) * 1080 / 1000000, uclk_states[i], 32);

		calculated_states[i].fabricclk_mhz = (min_fclk_required_by_uclk < min_dcfclk) ?
				min_dcfclk : min_fclk_required_by_uclk;

		calculated_states[i].socclk_mhz = (calculated_states[i].fabricclk_mhz > max_clocks->socClockInKhz / 1000) ?
				max_clocks->socClockInKhz / 1000 : calculated_states[i].fabricclk_mhz;

		calculated_states[i].dcfclk_mhz = (calculated_states[i].fabricclk_mhz > max_clocks->dcfClockInKhz / 1000) ?
				max_clocks->dcfClockInKhz / 1000 : calculated_states[i].fabricclk_mhz;

		calculated_states[i].dispclk_mhz = max_clocks->displayClockInKhz / 1000;
		calculated_states[i].dppclk_mhz = max_clocks->displayClockInKhz / 1000;
		calculated_states[i].dscclk_mhz = max_clocks->displayClockInKhz / (1000 * 3);

		calculated_states[i].phyclk_mhz = max_clocks->phyClockInKhz / 1000;

		num_calculated_states++;
	}

	calculated_states[num_calculated_states - 1].socclk_mhz = max_clocks->socClockInKhz / 1000;
	calculated_states[num_calculated_states - 1].fabricclk_mhz = max_clocks->socClockInKhz / 1000;
	calculated_states[num_calculated_states - 1].dcfclk_mhz = max_clocks->dcfClockInKhz / 1000;

	memcpy(bb->clock_limits, calculated_states, sizeof(bb->clock_limits));
	bb->num_states = num_calculated_states;

	// Duplicate the last state, DML always an extra state identical to max state to work
	memcpy(&bb->clock_limits[num_calculated_states], &bb->clock_limits[num_calculated_states - 1], sizeof(struct _vcs_dpi_voltage_scaling_st));
	bb->clock_limits[num_calculated_states].state = bb->num_states;
}