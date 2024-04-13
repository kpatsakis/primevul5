static inline void cil_reset_classperms(struct cil_classperms *cp)
{
	if (cp == NULL) {
		return;
	}

	cil_list_destroy(&cp->perms, CIL_FALSE);
}