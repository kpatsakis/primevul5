static inline void cil_reset_context(struct cil_context *context)
{
	if (context->range_str == NULL) {
		cil_reset_levelrange(context->range);
	}
}