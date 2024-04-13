R_API RBinJavaElementValuePair *r_bin_java_element_pair_new(ut8 *buffer, ut64 sz, ut64 buf_offset) {
	if (!buffer || sz < 4) {
		return NULL;
	}
	RBinJavaElementValuePair *evp = R_NEW0 (RBinJavaElementValuePair);
	if (!evp) {
		return NULL;
	}
	// TODO: What is the signifigance of evp element
	evp->element_name_idx = R_BIN_JAVA_USHORT (buffer, 0);
	ut64 offset = 2;
	evp->file_offset = buf_offset;
	evp->name = r_bin_java_get_utf8_from_bin_cp_list (R_BIN_JAVA_GLOBAL_BIN, evp->element_name_idx);
	if (!evp->name) {
		// TODO: eprintf unable to find the name for the given index
		eprintf ("ElementValue Name is invalid.\n");
		evp->name = strdup ("UNKNOWN");
	}
	if (offset >= sz) {
		free (evp);
		return NULL;
	}
	evp->value = r_bin_java_element_value_new (buffer + offset, sz - offset, buf_offset + offset);
	offset += evp->value->size;
	if (offset >= sz) {
		free (evp->value);
		free (evp);
		return NULL;
	}
	evp->size = offset;
	return evp;
}