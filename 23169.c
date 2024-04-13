static inline unsigned int xt_hooknum(const struct xt_action_param *par)
{
	return par->state->hook;
}