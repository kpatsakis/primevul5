static inline struct net_device *xt_out(const struct xt_action_param *par)
{
	return par->state->out;
}