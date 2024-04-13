R_API char *r_bin_java_print_methodhandle_cp_stringify(RBinJavaCPTypeObj *obj) {
	ut32 size = 255, consumed = 0;
	char *value = malloc (size);
	ut8 ref_kind = obj->info.cp_method_handle.reference_kind;
	if (value) {
		memset (value, 0, size);
		consumed = snprintf (value, size, "%d.0x%04"PFMT64x ".%s.%s.%d",
			obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
			R_BIN_JAVA_REF_METAS[ref_kind].name, obj->info.cp_method_handle.reference_index);
		if (consumed >= size - 1) {
			free (value);
			size += size >> 1;
			value = malloc (size);
			if (value) {
				memset (value, 0, size);
				(void)snprintf (value, size, "%d.0x%04"PFMT64x ".%s.%s.%d",
					obj->metas->ord, obj->file_offset + obj->loadaddr, ((RBinJavaCPTypeMetas *) obj->metas->type_info)->name,
					R_BIN_JAVA_REF_METAS[ref_kind].name, obj->info.cp_method_handle.reference_index);
			}
		}
	}
	return value;
}