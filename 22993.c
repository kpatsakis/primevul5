R_API RBinJavaAttrInfo *r_bin_java_read_next_attr_from_buffer(RBinJavaObj *bin, ut8 *buffer, st64 sz, st64 buf_offset) {
	RBinJavaAttrInfo *attr = NULL;
	ut64 offset = 0;
	ut16 name_idx;
	st64 nsz;

	if (!buffer || ((int) sz) < 4 || buf_offset < 0) {
		eprintf ("r_bin_Java_read_next_attr_from_buffer: invalid buffer size %d\n", (int) sz);
		return NULL;
	}
	name_idx = R_BIN_JAVA_USHORT (buffer, offset);
	offset += 2;
	nsz = R_BIN_JAVA_UINT (buffer, offset);
	// DEAD INCREMENT offset += 4;

	char *name = r_bin_java_get_utf8_from_bin_cp_list (R_BIN_JAVA_GLOBAL_BIN, name_idx);
	if (!name) {
		name = strdup ("unknown");
	}
	IFDBG eprintf("r_bin_java_read_next_attr: name_idx = %d is %s\n", name_idx, name);
	RBinJavaAttrMetas *type_info = r_bin_java_get_attr_type_by_name (name);
	if (type_info) {
		IFDBG eprintf("Typeinfo: %s, was %s\n", type_info->name, name);
		// printf ("SZ %d %d %d\n", nsz, sz, buf_offset);
		if (nsz > sz) {
			free (name);
			return NULL;
		}
		if ((attr = type_info->allocs->new_obj (bin, buffer, nsz, buf_offset))) {
			attr->metas->ord = (R_BIN_JAVA_GLOBAL_BIN->attr_idx++);
		}
	} else {
		eprintf ("r_bin_java_read_next_attr_from_buffer: Cannot find type_info for %s\n", name);
	}
	free (name);
	return attr;
}