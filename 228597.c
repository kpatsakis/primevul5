static inline void cil_reset_userlevel(struct cil_userlevel *userlevel)
{
	if (userlevel->level_str == NULL) {
		cil_reset_level(userlevel->level);
	}
}