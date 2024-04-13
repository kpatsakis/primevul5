static inline const char *xt_outname(const struct xt_action_param *par)
{
	return par->state->out->name;
}