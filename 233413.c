bool dcn20_fast_validate_bw(
		struct dc *dc,
		struct dc_state *context,
		display_e2e_pipe_params_st *pipes,
		int *pipe_cnt_out,
		int *pipe_split_from,
		int *vlevel_out)
{
	bool out = false;

	int pipe_cnt, i, pipe_idx, vlevel, vlevel_unsplit;
	bool odm_capable = context->bw_ctx.dml.ip.odm_capable;
	bool force_split = false;
#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
	bool failed_non_odm_dsc = false;
#endif
	int split_threshold = dc->res_pool->pipe_count / 2;
	bool avoid_split = dc->debug.pipe_split_policy != MPC_SPLIT_DYNAMIC;


	ASSERT(pipes);
	if (!pipes)
		return false;

	/* merge previously split odm pipes since mode support needs to make the decision */
	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		struct pipe_ctx *pipe = &context->res_ctx.pipe_ctx[i];
		struct pipe_ctx *odm_pipe = pipe->next_odm_pipe;

		if (pipe->prev_odm_pipe)
			continue;

		pipe->next_odm_pipe = NULL;
		while (odm_pipe) {
			struct pipe_ctx *next_odm_pipe = odm_pipe->next_odm_pipe;

			odm_pipe->plane_state = NULL;
			odm_pipe->stream = NULL;
			odm_pipe->top_pipe = NULL;
			odm_pipe->bottom_pipe = NULL;
			odm_pipe->prev_odm_pipe = NULL;
			odm_pipe->next_odm_pipe = NULL;
#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
			if (odm_pipe->stream_res.dsc)
				release_dsc(&context->res_ctx, dc->res_pool, &odm_pipe->stream_res.dsc);
#endif
			/* Clear plane_res and stream_res */
			memset(&odm_pipe->plane_res, 0, sizeof(odm_pipe->plane_res));
			memset(&odm_pipe->stream_res, 0, sizeof(odm_pipe->stream_res));
			odm_pipe = next_odm_pipe;
		}
		if (pipe->plane_state)
			resource_build_scaling_params(pipe);
	}

	/* merge previously mpc split pipes since mode support needs to make the decision */
	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		struct pipe_ctx *pipe = &context->res_ctx.pipe_ctx[i];
		struct pipe_ctx *hsplit_pipe = pipe->bottom_pipe;

		if (!hsplit_pipe || hsplit_pipe->plane_state != pipe->plane_state)
			continue;

		pipe->bottom_pipe = hsplit_pipe->bottom_pipe;
		if (hsplit_pipe->bottom_pipe)
			hsplit_pipe->bottom_pipe->top_pipe = pipe;
		hsplit_pipe->plane_state = NULL;
		hsplit_pipe->stream = NULL;
		hsplit_pipe->top_pipe = NULL;
		hsplit_pipe->bottom_pipe = NULL;

		/* Clear plane_res and stream_res */
		memset(&hsplit_pipe->plane_res, 0, sizeof(hsplit_pipe->plane_res));
		memset(&hsplit_pipe->stream_res, 0, sizeof(hsplit_pipe->stream_res));
		if (pipe->plane_state)
			resource_build_scaling_params(pipe);
	}

	if (dc->res_pool->funcs->populate_dml_pipes)
		pipe_cnt = dc->res_pool->funcs->populate_dml_pipes(dc,
			&context->res_ctx, pipes);
	else
		pipe_cnt = dcn20_populate_dml_pipes_from_context(dc,
			&context->res_ctx, pipes);

	*pipe_cnt_out = pipe_cnt;

	if (!pipe_cnt) {
		out = true;
		goto validate_out;
	}

	context->bw_ctx.dml.ip.odm_capable = 0;

	vlevel = dml_get_voltage_level(&context->bw_ctx.dml, pipes, pipe_cnt);

	context->bw_ctx.dml.ip.odm_capable = odm_capable;

#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
	/* 1 dsc per stream dsc validation */
	if (vlevel <= context->bw_ctx.dml.soc.num_states)
		if (!dcn20_validate_dsc(dc, context)) {
			failed_non_odm_dsc = true;
			vlevel = context->bw_ctx.dml.soc.num_states + 1;
		}
#endif

	if (vlevel > context->bw_ctx.dml.soc.num_states && odm_capable)
		vlevel = dml_get_voltage_level(&context->bw_ctx.dml, pipes, pipe_cnt);

	if (vlevel > context->bw_ctx.dml.soc.num_states)
		goto validate_fail;

	if ((context->stream_count > split_threshold && dc->current_state->stream_count <= split_threshold)
		|| (context->stream_count <= split_threshold && dc->current_state->stream_count > split_threshold))
		context->commit_hints.full_update_needed = true;

	/*initialize pipe_just_split_from to invalid idx*/
	for (i = 0; i < MAX_PIPES; i++)
		pipe_split_from[i] = -1;

	/* Single display only conditionals get set here */
	for (i = 0; i < dc->res_pool->pipe_count; i++) {
		struct pipe_ctx *pipe = &context->res_ctx.pipe_ctx[i];
		bool exit_loop = false;

		if (!pipe->stream || pipe->top_pipe)
			continue;

		if (dc->debug.force_single_disp_pipe_split) {
			if (!force_split)
				force_split = true;
			else {
				force_split = false;
				exit_loop = true;
			}
		}
		if (dc->debug.pipe_split_policy == MPC_SPLIT_AVOID_MULT_DISP) {
			if (avoid_split)
				avoid_split = false;
			else {
				avoid_split = true;
				exit_loop = true;
			}
		}
		if (exit_loop)
			break;
	}

	if (context->stream_count > split_threshold)
		avoid_split = true;

	vlevel_unsplit = vlevel;
	for (i = 0, pipe_idx = 0; i < dc->res_pool->pipe_count; i++) {
		if (!context->res_ctx.pipe_ctx[i].stream)
			continue;
		for (; vlevel_unsplit <= context->bw_ctx.dml.soc.num_states; vlevel_unsplit++)
			if (context->bw_ctx.dml.vba.NoOfDPP[vlevel_unsplit][0][pipe_idx] == 1)
				break;
		pipe_idx++;
	}

	for (i = 0, pipe_idx = -1; i < dc->res_pool->pipe_count; i++) {
		struct pipe_ctx *pipe = &context->res_ctx.pipe_ctx[i];
		struct pipe_ctx *hsplit_pipe = pipe->bottom_pipe;
		bool need_split = true;
		bool need_split3d;

		if (!pipe->stream || pipe_split_from[i] >= 0)
			continue;

		pipe_idx++;

		if (dc->debug.force_odm_combine & (1 << pipe->stream_res.tg->inst)) {
			force_split = true;
			context->bw_ctx.dml.vba.ODMCombineEnabled[pipe_idx] = true;
			context->bw_ctx.dml.vba.ODMCombineEnablePerState[vlevel][pipe_idx] = true;
		}
		if (force_split && context->bw_ctx.dml.vba.NoOfDPP[vlevel][context->bw_ctx.dml.vba.maxMpcComb][pipe_idx] == 1)
			context->bw_ctx.dml.vba.RequiredDPPCLK[vlevel][context->bw_ctx.dml.vba.maxMpcComb][pipe_idx] /= 2;
		if (!pipe->top_pipe && !pipe->plane_state && context->bw_ctx.dml.vba.ODMCombineEnabled[pipe_idx]) {
			hsplit_pipe = dcn20_find_secondary_pipe(dc, &context->res_ctx, dc->res_pool, pipe);
			ASSERT(hsplit_pipe);
			if (!dcn20_split_stream_for_odm(
					&context->res_ctx, dc->res_pool,
					pipe, hsplit_pipe))
				goto validate_fail;
			pipe_split_from[hsplit_pipe->pipe_idx] = pipe_idx;
			dcn20_build_mapped_resource(dc, context, pipe->stream);
		}

		if (!pipe->plane_state)
			continue;
		/* Skip 2nd half of already split pipe */
		if (pipe->top_pipe && pipe->plane_state == pipe->top_pipe->plane_state)
			continue;

		need_split3d = ((pipe->stream->view_format ==
				VIEW_3D_FORMAT_SIDE_BY_SIDE ||
				pipe->stream->view_format ==
				VIEW_3D_FORMAT_TOP_AND_BOTTOM) &&
				(pipe->stream->timing.timing_3d_format ==
				TIMING_3D_FORMAT_TOP_AND_BOTTOM ||
				 pipe->stream->timing.timing_3d_format ==
				TIMING_3D_FORMAT_SIDE_BY_SIDE));

		if (avoid_split && vlevel_unsplit <= context->bw_ctx.dml.soc.num_states && !force_split && !need_split3d) {
			need_split = false;
			vlevel = vlevel_unsplit;
			context->bw_ctx.dml.vba.maxMpcComb = 0;
		} else
			need_split = context->bw_ctx.dml.vba.NoOfDPP[vlevel][context->bw_ctx.dml.vba.maxMpcComb][pipe_idx] == 2;

		/* We do not support mpo + odm at the moment */
		if (hsplit_pipe && hsplit_pipe->plane_state != pipe->plane_state
				&& context->bw_ctx.dml.vba.ODMCombineEnabled[pipe_idx])
			goto validate_fail;

		if (need_split3d || need_split || force_split) {
			if (!hsplit_pipe || hsplit_pipe->plane_state != pipe->plane_state) {
				/* pipe not split previously needs split */
				hsplit_pipe = dcn20_find_secondary_pipe(dc, &context->res_ctx, dc->res_pool, pipe);
				ASSERT(hsplit_pipe || force_split);
				if (!hsplit_pipe)
					continue;

				if (context->bw_ctx.dml.vba.ODMCombineEnabled[pipe_idx]) {
					if (!dcn20_split_stream_for_odm(
							&context->res_ctx, dc->res_pool,
							pipe, hsplit_pipe))
						goto validate_fail;
				} else
					dcn20_split_stream_for_mpc(
						&context->res_ctx, dc->res_pool,
						pipe, hsplit_pipe);
				pipe_split_from[hsplit_pipe->pipe_idx] = pipe_idx;
			}
		} else if (hsplit_pipe && hsplit_pipe->plane_state == pipe->plane_state) {
			/* merge should already have been done */
			ASSERT(0);
		}
	}
#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
	/* Actual dsc count per stream dsc validation*/
	if (failed_non_odm_dsc && !dcn20_validate_dsc(dc, context)) {
		context->bw_ctx.dml.vba.ValidationStatus[context->bw_ctx.dml.vba.soc.num_states] =
				DML_FAIL_DSC_VALIDATION_FAILURE;
		goto validate_fail;
	}
#endif

	*vlevel_out = vlevel;

	out = true;
	goto validate_out;

validate_fail:
	out = false;

validate_out:
	return out;
}