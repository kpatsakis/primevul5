static inline void cil_reset_level(struct cil_level *level)
{
	cil_reset_cats(level->cats);
}