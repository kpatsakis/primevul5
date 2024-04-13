static inline void cil_reset_levelrange(struct cil_levelrange *levelrange)
{
	if (levelrange->low_str == NULL) {
		cil_reset_level(levelrange->low);
	}

	if (levelrange->high_str == NULL) {
		cil_reset_level(levelrange->high);
	}
}