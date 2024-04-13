static inline void cil_reset_classperms_list(struct cil_list *cp_list)
{
	struct cil_list_item *curr;

	if (cp_list == NULL) {
		return;
	}

	cil_list_for_each(curr, cp_list) {
		if (curr->flavor == CIL_CLASSPERMS) { /* KERNEL or MAP */
			cil_reset_classperms(curr->data);
		} else if (curr->flavor == CIL_CLASSPERMS_SET) { /* SET */
			cil_reset_classperms_set(curr->data);
		}
	}
}