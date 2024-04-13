R_API RBinJavaAttrInfo *r_bin_java_default_attr_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz, ut64 buf_offset) {
	// NOTE: this function receives the buffer offset in the original buffer,
	// but the buffer is already point to that particular offset.
	// XXX - all the code that relies on this function should probably be modified
	// so that the original buffer pointer is passed in and then the buffer+buf_offset
	// points to the correct location.
	RBinJavaAttrInfo *attr = R_NEW0 (RBinJavaAttrInfo);
	if (!attr) {
		return NULL;
	}
	RBinJavaAttrMetas *type_info = NULL;
	attr->metas = R_NEW0 (RBinJavaMetaInfo);
	if (attr->metas == NULL) {
		free (attr);
		return NULL;
	}
	attr->is_attr_in_old_format = r_bin_java_is_old_format(bin);
	attr->file_offset = buf_offset;
	attr->name_idx = R_BIN_JAVA_USHORT (buffer, 0);
	attr->length = R_BIN_JAVA_UINT (buffer, 2);
	attr->size = R_BIN_JAVA_UINT (buffer, 2) + 6;
	attr->name = r_bin_java_get_utf8_from_bin_cp_list (R_BIN_JAVA_GLOBAL_BIN, attr->name_idx);
	if (attr->name == NULL) {
		// Something bad has happened
		attr->name = r_str_dup (NULL, "NULL");
		eprintf ("r_bin_java_default_attr_new: Unable to find the name for %d index.\n", attr->name_idx);
	}
	type_info = r_bin_java_get_attr_type_by_name (attr->name);
	attr->metas->ord = (R_BIN_JAVA_GLOBAL_BIN->attr_idx++);
	attr->metas->type_info = (void *) type_info;
	// IFDBG eprintf ("	Addrs for type_info [tag=%d]: 0x%08"PFMT64x"\n", type_val, &attr->metas->type_info);
	return attr;
}