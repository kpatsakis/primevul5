R_API RBinSymbol *r_bin_java_create_new_symbol_from_ref(RBinJavaObj *bin, RBinJavaCPTypeObj *obj, ut64 baddr) {
	RBinSymbol *sym = R_NEW0 (RBinSymbol);
	if (!sym) {
		return NULL;
	}
	char *class_name, *name, *type_name;
	if (obj == NULL || (obj->tag != R_BIN_JAVA_CP_METHODREF &&
	obj->tag != R_BIN_JAVA_CP_INTERFACEMETHOD_REF &&
	obj->tag != R_BIN_JAVA_CP_FIELDREF)) {
		R_FREE (sym);
		return sym;
	}
	if (sym) {
		class_name = r_bin_java_get_name_from_bin_cp_list (bin,
			obj->info.cp_method.class_idx);
		name = r_bin_java_get_name_from_bin_cp_list (bin,
			obj->info.cp_method.name_and_type_idx);
		type_name = r_bin_java_get_name_from_bin_cp_list (bin,
			obj->info.cp_method.name_and_type_idx);
		if (name) {
			sym->name = name;
			name = NULL;
		}
		if (type_name) {
			sym->type = r_str_constpool_get (&bin->constpool, type_name);
			R_FREE (type_name);
		}
		if (class_name) {
			sym->classname = strdup (class_name);
		}
		sym->paddr = obj->file_offset + baddr;
		sym->vaddr = obj->file_offset + baddr;
		sym->ordinal = obj->metas->ord;
		sym->size = 0;
	}
	return sym;
}