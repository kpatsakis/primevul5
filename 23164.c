static inline u_int8_t xt_family(const struct xt_action_param *par)
{
	return par->state->pf;
}