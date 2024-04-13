R_API RBinSymbol *r_bin_java_create_new_symbol_from_field(RBinJavaField *fm_type, ut64 baddr) {
	RBinSymbol *sym = R_NEW0 (RBinSymbol);
	if (fm_type == NULL || fm_type->field_ref_cp_obj == NULL || fm_type->field_ref_cp_obj == &R_BIN_JAVA_NULL_TYPE) {
		R_FREE (sym);
	}
	if (sym) {
		sym->name = strdup (fm_type->name);
		// strncpy (sym->type, fm_type->descriptor, R_BIN_SIZEOF_STRINGS);
		if (fm_type->type == R_BIN_JAVA_FIELD_TYPE_METHOD) {
			sym->type = R_BIN_TYPE_FUNC_STR;
			sym->paddr = r_bin_java_get_method_code_offset (fm_type);
			sym->vaddr = r_bin_java_get_method_code_offset (fm_type) + baddr;
			sym->size = r_bin_java_get_method_code_size (fm_type);
		} else {
			sym->type = "FIELD";
			sym->paddr = fm_type->file_offset;// r_bin_java_get_method_code_offset (fm_type);
			sym->vaddr = fm_type->file_offset + baddr;
			sym->size = fm_type->size;
		}
		if (r_bin_java_is_fm_type_protected (fm_type)) {
			sym->bind = R_BIN_BIND_LOCAL_STR;
		} else if (r_bin_java_is_fm_type_private (fm_type)) {
			sym->bind = R_BIN_BIND_LOCAL_STR;
		} else if (r_bin_java_is_fm_type_protected (fm_type)) {
			sym->bind = R_BIN_BIND_GLOBAL_STR;
		}
		sym->forwarder = "NONE";
		if (fm_type->class_name) {
			sym->classname = strdup (fm_type->class_name);
		} else {
			sym->classname = strdup ("UNKNOWN"); // dupped names?
		}
		sym->ordinal = fm_type->metas->ord;
		sym->visibility = fm_type->flags;
		if (fm_type->flags_str) {
			sym->visibility_str = strdup (fm_type->flags_str);
		}
	}
	return sym;
}