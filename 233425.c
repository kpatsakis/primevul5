static bool init_soc_bounding_box(struct dc *dc,
				  struct dcn20_resource_pool *pool)
{
	const struct gpu_info_soc_bounding_box_v1_0 *bb = dc->soc_bounding_box;
	struct _vcs_dpi_soc_bounding_box_st *loaded_bb =
			get_asic_rev_soc_bb(dc->ctx->asic_id.hw_internal_rev);
	struct _vcs_dpi_ip_params_st *loaded_ip =
			get_asic_rev_ip_params(dc->ctx->asic_id.hw_internal_rev);

	DC_LOGGER_INIT(dc->ctx->logger);

	if (!bb && !SOC_BOUNDING_BOX_VALID) {
		DC_LOG_ERROR("%s: not valid soc bounding box/n", __func__);
		return false;
	}

	if (bb && !SOC_BOUNDING_BOX_VALID) {
		int i;

		dcn2_0_nv12_soc.sr_exit_time_us =
				fixed16_to_double_to_cpu(bb->sr_exit_time_us);
		dcn2_0_nv12_soc.sr_enter_plus_exit_time_us =
				fixed16_to_double_to_cpu(bb->sr_enter_plus_exit_time_us);
		dcn2_0_nv12_soc.urgent_latency_us =
				fixed16_to_double_to_cpu(bb->urgent_latency_us);
		dcn2_0_nv12_soc.urgent_latency_pixel_data_only_us =
				fixed16_to_double_to_cpu(bb->urgent_latency_pixel_data_only_us);
		dcn2_0_nv12_soc.urgent_latency_pixel_mixed_with_vm_data_us =
				fixed16_to_double_to_cpu(bb->urgent_latency_pixel_mixed_with_vm_data_us);
		dcn2_0_nv12_soc.urgent_latency_vm_data_only_us =
				fixed16_to_double_to_cpu(bb->urgent_latency_vm_data_only_us);
		dcn2_0_nv12_soc.urgent_out_of_order_return_per_channel_pixel_only_bytes =
				le32_to_cpu(bb->urgent_out_of_order_return_per_channel_pixel_only_bytes);
		dcn2_0_nv12_soc.urgent_out_of_order_return_per_channel_pixel_and_vm_bytes =
				le32_to_cpu(bb->urgent_out_of_order_return_per_channel_pixel_and_vm_bytes);
		dcn2_0_nv12_soc.urgent_out_of_order_return_per_channel_vm_only_bytes =
				le32_to_cpu(bb->urgent_out_of_order_return_per_channel_vm_only_bytes);
		dcn2_0_nv12_soc.pct_ideal_dram_sdp_bw_after_urgent_pixel_only =
				fixed16_to_double_to_cpu(bb->pct_ideal_dram_sdp_bw_after_urgent_pixel_only);
		dcn2_0_nv12_soc.pct_ideal_dram_sdp_bw_after_urgent_pixel_and_vm =
				fixed16_to_double_to_cpu(bb->pct_ideal_dram_sdp_bw_after_urgent_pixel_and_vm);
		dcn2_0_nv12_soc.pct_ideal_dram_sdp_bw_after_urgent_vm_only =
				fixed16_to_double_to_cpu(bb->pct_ideal_dram_sdp_bw_after_urgent_vm_only);
		dcn2_0_nv12_soc.max_avg_sdp_bw_use_normal_percent =
				fixed16_to_double_to_cpu(bb->max_avg_sdp_bw_use_normal_percent);
		dcn2_0_nv12_soc.max_avg_dram_bw_use_normal_percent =
				fixed16_to_double_to_cpu(bb->max_avg_dram_bw_use_normal_percent);
		dcn2_0_nv12_soc.writeback_latency_us =
				fixed16_to_double_to_cpu(bb->writeback_latency_us);
		dcn2_0_nv12_soc.ideal_dram_bw_after_urgent_percent =
				fixed16_to_double_to_cpu(bb->ideal_dram_bw_after_urgent_percent);
		dcn2_0_nv12_soc.max_request_size_bytes =
				le32_to_cpu(bb->max_request_size_bytes);
		dcn2_0_nv12_soc.dram_channel_width_bytes =
				le32_to_cpu(bb->dram_channel_width_bytes);
		dcn2_0_nv12_soc.fabric_datapath_to_dcn_data_return_bytes =
				le32_to_cpu(bb->fabric_datapath_to_dcn_data_return_bytes);
		dcn2_0_nv12_soc.dcn_downspread_percent =
				fixed16_to_double_to_cpu(bb->dcn_downspread_percent);
		dcn2_0_nv12_soc.downspread_percent =
				fixed16_to_double_to_cpu(bb->downspread_percent);
		dcn2_0_nv12_soc.dram_page_open_time_ns =
				fixed16_to_double_to_cpu(bb->dram_page_open_time_ns);
		dcn2_0_nv12_soc.dram_rw_turnaround_time_ns =
				fixed16_to_double_to_cpu(bb->dram_rw_turnaround_time_ns);
		dcn2_0_nv12_soc.dram_return_buffer_per_channel_bytes =
				le32_to_cpu(bb->dram_return_buffer_per_channel_bytes);
		dcn2_0_nv12_soc.round_trip_ping_latency_dcfclk_cycles =
				le32_to_cpu(bb->round_trip_ping_latency_dcfclk_cycles);
		dcn2_0_nv12_soc.urgent_out_of_order_return_per_channel_bytes =
				le32_to_cpu(bb->urgent_out_of_order_return_per_channel_bytes);
		dcn2_0_nv12_soc.channel_interleave_bytes =
				le32_to_cpu(bb->channel_interleave_bytes);
		dcn2_0_nv12_soc.num_banks =
				le32_to_cpu(bb->num_banks);
		dcn2_0_nv12_soc.num_chans =
				le32_to_cpu(bb->num_chans);
		dcn2_0_nv12_soc.vmm_page_size_bytes =
				le32_to_cpu(bb->vmm_page_size_bytes);
		dcn2_0_nv12_soc.dram_clock_change_latency_us =
				fixed16_to_double_to_cpu(bb->dram_clock_change_latency_us);
		// HACK!! Lower uclock latency switch time so we don't switch
		dcn2_0_nv12_soc.dram_clock_change_latency_us = 10;
		dcn2_0_nv12_soc.writeback_dram_clock_change_latency_us =
				fixed16_to_double_to_cpu(bb->writeback_dram_clock_change_latency_us);
		dcn2_0_nv12_soc.return_bus_width_bytes =
				le32_to_cpu(bb->return_bus_width_bytes);
		dcn2_0_nv12_soc.dispclk_dppclk_vco_speed_mhz =
				le32_to_cpu(bb->dispclk_dppclk_vco_speed_mhz);
		dcn2_0_nv12_soc.xfc_bus_transport_time_us =
				le32_to_cpu(bb->xfc_bus_transport_time_us);
		dcn2_0_nv12_soc.xfc_xbuf_latency_tolerance_us =
				le32_to_cpu(bb->xfc_xbuf_latency_tolerance_us);
		dcn2_0_nv12_soc.use_urgent_burst_bw =
				le32_to_cpu(bb->use_urgent_burst_bw);
		dcn2_0_nv12_soc.num_states =
				le32_to_cpu(bb->num_states);

		for (i = 0; i < dcn2_0_nv12_soc.num_states; i++) {
			dcn2_0_nv12_soc.clock_limits[i].state =
					le32_to_cpu(bb->clock_limits[i].state);
			dcn2_0_nv12_soc.clock_limits[i].dcfclk_mhz =
					fixed16_to_double_to_cpu(bb->clock_limits[i].dcfclk_mhz);
			dcn2_0_nv12_soc.clock_limits[i].fabricclk_mhz =
					fixed16_to_double_to_cpu(bb->clock_limits[i].fabricclk_mhz);
			dcn2_0_nv12_soc.clock_limits[i].dispclk_mhz =
					fixed16_to_double_to_cpu(bb->clock_limits[i].dispclk_mhz);
			dcn2_0_nv12_soc.clock_limits[i].dppclk_mhz =
					fixed16_to_double_to_cpu(bb->clock_limits[i].dppclk_mhz);
			dcn2_0_nv12_soc.clock_limits[i].phyclk_mhz =
					fixed16_to_double_to_cpu(bb->clock_limits[i].phyclk_mhz);
			dcn2_0_nv12_soc.clock_limits[i].socclk_mhz =
					fixed16_to_double_to_cpu(bb->clock_limits[i].socclk_mhz);
			dcn2_0_nv12_soc.clock_limits[i].dscclk_mhz =
					fixed16_to_double_to_cpu(bb->clock_limits[i].dscclk_mhz);
			dcn2_0_nv12_soc.clock_limits[i].dram_speed_mts =
					fixed16_to_double_to_cpu(bb->clock_limits[i].dram_speed_mts);
		}
	}

	if (pool->base.pp_smu) {
		struct pp_smu_nv_clock_table max_clocks = {0};
		unsigned int uclk_states[8] = {0};
		unsigned int num_states = 0;
		enum pp_smu_status status;
		bool clock_limits_available = false;
		bool uclk_states_available = false;

		if (pool->base.pp_smu->nv_funcs.get_uclk_dpm_states) {
			status = (pool->base.pp_smu->nv_funcs.get_uclk_dpm_states)
				(&pool->base.pp_smu->nv_funcs.pp_smu, uclk_states, &num_states);

			uclk_states_available = (status == PP_SMU_RESULT_OK);
		}

		if (pool->base.pp_smu->nv_funcs.get_maximum_sustainable_clocks) {
			status = (*pool->base.pp_smu->nv_funcs.get_maximum_sustainable_clocks)
					(&pool->base.pp_smu->nv_funcs.pp_smu, &max_clocks);
			/* SMU cannot set DCF clock to anything equal to or higher than SOC clock
			 */
			if (max_clocks.dcfClockInKhz >= max_clocks.socClockInKhz)
				max_clocks.dcfClockInKhz = max_clocks.socClockInKhz - 1000;
			clock_limits_available = (status == PP_SMU_RESULT_OK);
		}

		if (clock_limits_available && uclk_states_available && num_states)
			update_bounding_box(dc, loaded_bb, &max_clocks, uclk_states, num_states);
		else if (clock_limits_available)
			cap_soc_clocks(loaded_bb, max_clocks);
	}

	loaded_ip->max_num_otg = pool->base.res_cap->num_timing_generator;
	loaded_ip->max_num_dpp = pool->base.pipe_count;
	patch_bounding_box(dc, loaded_bb);

	return true;
}