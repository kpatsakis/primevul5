static inline struct net_device *xt_in(const struct xt_action_param *par)
{
	return par->state->in;
}