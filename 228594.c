static inline void cil_reset_userrange(struct cil_userrange *userrange)
{
	if (userrange->range_str == NULL) {
		cil_reset_levelrange(userrange->range);
	}
}