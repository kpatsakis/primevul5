struct pp_smu_funcs *dcn20_pp_smu_create(struct dc_context *ctx)
{
	struct pp_smu_funcs *pp_smu = kzalloc(sizeof(*pp_smu), GFP_KERNEL);

	if (!pp_smu)
		return pp_smu;

	dm_pp_get_funcs(ctx, pp_smu);

	if (pp_smu->ctx.ver != PP_SMU_VER_NV)
		pp_smu = memset(pp_smu, 0, sizeof(struct pp_smu_funcs));

	return pp_smu;
}