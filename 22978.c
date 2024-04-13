R_API ConstJavaValue *U(r_bin_java_resolve_to_const_value)(RBinJavaObj * BIN_OBJ, int idx) {
	// TODO XXX FIXME add a size parameter to the str when it is passed in
	RBinJavaCPTypeObj *item = NULL, *item2 = NULL;
	ConstJavaValue *result = R_NEW0 (ConstJavaValue);
	if (!result) {
		return NULL;
	}
	char *class_str = NULL,
	*name_str = NULL,
	*desc_str = NULL,
	*string_str = NULL,
	*empty = "",
	*cp_name = NULL;
	result->type = "unknown";
	if (BIN_OBJ && BIN_OBJ->cp_count < 1) {
		// r_bin_java_new_bin(BIN_OBJ);
		return result;
	}
	item = (RBinJavaCPTypeObj *) r_bin_java_get_item_from_bin_cp_list (BIN_OBJ, idx);
	if (!item) {
		return result;
	}
	cp_name = ((RBinJavaCPTypeMetas *) item->metas->type_info)->name;
	IFDBG eprintf("java_resolve Resolved: (%d) %s\n", idx, cp_name);
	if (strcmp (cp_name, "Class") == 0) {
		item2 = (RBinJavaCPTypeObj *) r_bin_java_get_item_from_bin_cp_list (BIN_OBJ, idx);
		// str = r_bin_java_get_name_from_bin_cp_list (BIN_OBJ, idx-1);
		class_str = r_bin_java_get_item_name_from_bin_cp_list (BIN_OBJ, item);
		if (!class_str) {
			class_str = empty;
		}
		name_str = r_bin_java_get_item_name_from_bin_cp_list (BIN_OBJ, item2);
		if (!name_str) {
			name_str = empty;
		}
		desc_str = r_bin_java_get_item_desc_from_bin_cp_list (BIN_OBJ, item2);
		if (!desc_str) {
			desc_str = empty;
		}
		result->value._ref = R_NEW0 (_JavaRef);
		result->type = "ref";
		result->value._ref->class_name = strdup (class_str);
		result->value._ref->name = strdup (name_str);
		result->value._ref->desc = strdup (desc_str);
		if (class_str != empty) {
			free (class_str);
		}
		if (name_str != empty) {
			free (name_str);
		}
		if (desc_str != empty) {
			free (desc_str);
		}
	} else if (strcmp (cp_name, "MethodRef") == 0 ||
	strcmp (cp_name, "FieldRef") == 0 ||
	strcmp (cp_name, "InterfaceMethodRef") == 0) {
		/*
		*  The MethodRef, FieldRef, and InterfaceMethodRef structures
		*/
		class_str = r_bin_java_get_name_from_bin_cp_list (BIN_OBJ, item->info.cp_method.class_idx);
		if (!class_str) {
			class_str = empty;
		}
		name_str = r_bin_java_get_item_name_from_bin_cp_list (BIN_OBJ, item);
		if (!name_str) {
			name_str = empty;
		}
		desc_str = r_bin_java_get_item_desc_from_bin_cp_list (BIN_OBJ, item);
		if (!desc_str) {
			desc_str = empty;
		}
		result->value._ref = R_NEW0 (_JavaRef);
		result->type = "ref";
		result->value._ref->class_name = strdup (class_str);
		result->value._ref->name = strdup (name_str);
		result->value._ref->desc = strdup (desc_str);
		if (class_str != empty) {
			free (class_str);
		}
		if (name_str != empty) {
			free (name_str);
		}
		if (desc_str != empty) {
			free (desc_str);
		}
	} else if (strcmp (cp_name, "String") == 0) {
		ut32 length = r_bin_java_get_utf8_len_from_bin_cp_list (BIN_OBJ, item->info.cp_string.string_idx);
		string_str = r_bin_java_get_utf8_from_bin_cp_list (BIN_OBJ, item->info.cp_string.string_idx);
		IFDBG eprintf("java_resolve String got: (%d) %s\n", item->info.cp_string.string_idx, string_str);
		if (!string_str) {
			string_str = empty;
			length = strlen (empty);
		}
		result->type = "str";
		result->value._str = R_NEW0 (struct  java_const_value_str_t);
		result->value._str->len = length;
		if (length > 0) {
			result->value._str->str = r_str_ndup (string_str, length);
		} else {
			result->value._str->str = strdup ("");
		}
		if (string_str != empty) {
			free (string_str);
		}
	} else if (strcmp (cp_name, "Utf8") == 0) {
		result->type = "str";
		result->value._str = R_NEW0 (struct java_const_value_str_t);
		result->value._str->str = malloc (item->info.cp_utf8.length);
		result->value._str->len = item->info.cp_utf8.length;
		memcpy (result->value._str->str, item->info.cp_utf8.bytes, item->info.cp_utf8.length);
	} else if (strcmp (cp_name, "Long") == 0) {
		result->type = "long";
		result->value._long = r_bin_java_raw_to_long (item->info.cp_long.bytes.raw, 0);
	} else if (strcmp (cp_name, "Double") == 0) {
		result->type = "double";
		result->value._double = r_bin_java_raw_to_double (item->info.cp_double.bytes.raw, 0);
	} else if (strcmp (cp_name, "Integer") == 0) {
		result->type = "int";
		result->value._int = R_BIN_JAVA_UINT (item->info.cp_integer.bytes.raw, 0);
	} else if (strcmp (cp_name, "Float") == 0) {
		result->type = "float";
		result->value._float = R_BIN_JAVA_FLOAT (item->info.cp_float.bytes.raw, 0);
	} else if (strcmp (cp_name, "NameAndType") == 0) {
		result->value._ref = R_NEW0 (struct java_const_value_ref_t);
		result->type = "ref";
		name_str = r_bin_java_get_item_name_from_bin_cp_list (BIN_OBJ, item);
		if (!name_str) {
			name_str = empty;
		}
		desc_str = r_bin_java_get_item_desc_from_bin_cp_list (BIN_OBJ, item);
		if (!desc_str) {
			desc_str = empty;
		}
		result->value._ref->class_name = strdup (empty);
		result->value._ref->name = strdup (name_str);
		result->value._ref->desc = strdup (desc_str);
		if (name_str != empty) {
			free (name_str);
		}
		if (desc_str != empty) {
			free (desc_str);
		}
		result->value._ref->is_method = r_bin_java_does_cp_idx_ref_method (BIN_OBJ, idx);
		result->value._ref->is_field = r_bin_java_does_cp_idx_ref_field (BIN_OBJ, idx);
	}
	return result;
}