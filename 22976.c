R_API RBinJavaAttrInfo *r_bin_java_signature_attr_new(RBinJavaObj *bin, ut8 *buffer, ut64 sz, ut64 buf_offset) {
	ut64 offset = 6;
	RBinJavaAttrInfo *attr = r_bin_java_default_attr_new (bin, buffer, sz, buf_offset);
	if (!attr) {
		return NULL;
	}
	attr->type = R_BIN_JAVA_ATTR_TYPE_SIGNATURE_ATTR;
	// attr->info.source_file_attr.sourcefile_idx = R_BIN_JAVA_USHORT (buffer, offset);
	// offset += 2;
	attr->info.signature_attr.signature_idx = R_BIN_JAVA_USHORT (buffer, offset);
	offset += 2;
	attr->info.signature_attr.signature = r_bin_java_get_utf8_from_bin_cp_list (
		R_BIN_JAVA_GLOBAL_BIN, attr->info.signature_attr.signature_idx);
	if (!attr->info.signature_attr.signature) {
		eprintf ("r_bin_java_signature_attr_new: Unable to resolve the "
			"Signature UTF8 String Index: 0x%02x\n", attr->info.signature_attr.signature_idx);
	}
	attr->size = offset;
	// IFDBG r_bin_java_print_source_code_file_attr_summary(attr);
	return attr;
}