R_API bool sdb_iterate_build_list(void *user, const char *k, const char *v) {
	RList *bin_objs_list = (RList *) user;
	size_t value = (size_t) sdb_atoi (v);
	RBinJavaObj *bin_obj = NULL;
	IFDBG eprintf("Found %s == %"PFMT64x " bin_objs db\n", k, (ut64) value);
	if (value != 0 && value != (size_t) -1) {
		bin_obj = (RBinJavaObj *) value;
		r_list_append (bin_objs_list, bin_obj);
	}
	return true;
}