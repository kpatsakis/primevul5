struct dce_i2c_sw *dce80_i2c_sw_create(
	struct dc_context *ctx)
{
	struct dce_i2c_sw *dce_i2c_sw =
		kzalloc(sizeof(struct dce_i2c_sw), GFP_KERNEL);

	if (!dce_i2c_sw)
		return NULL;

	dce_i2c_sw_construct(dce_i2c_sw, ctx);

	return dce_i2c_sw;
}