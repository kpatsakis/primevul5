R_API char *r_bin_java_resolve(RBinJavaObj *BIN_OBJ, int idx, ut8 space_bn_name_type) {
	// TODO XXX FIXME add a size parameter to the str when it is passed in
	RBinJavaCPTypeObj *item = NULL, *item2 = NULL;
	char *class_str = NULL,
	*name_str = NULL,
	*desc_str = NULL,
	*string_str = NULL,
	*empty = "",
	*cp_name = NULL,
	*str = NULL;
	if (BIN_OBJ && BIN_OBJ->cp_count < 1) {
		// r_bin_java_new_bin(BIN_OBJ);
		return NULL;
	}
	item = (RBinJavaCPTypeObj *) r_bin_java_get_item_from_bin_cp_list (BIN_OBJ, idx);
	if (item) {
		cp_name = ((RBinJavaCPTypeMetas *) item->metas->type_info)->name;
		IFDBG eprintf("java_resolve Resolved: (%d) %s\n", idx, cp_name);
	} else {
		str = malloc (512);
		if (str) {
			snprintf (str, 512, "(%d) INVALID CP_OBJ", idx);
		}
		return str;
	}
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
		str = r_str_newf ("%s%s%s", name_str,
			space_bn_name_type ? " " : "", desc_str);
		if (class_str != empty) {
			free (class_str);
		}
		if (name_str != empty) {
			free (name_str);
		}
		if (desc_str != empty) {
			free (desc_str);
		}
	} else if (!strcmp (cp_name, "MethodRef") ||
	!strcmp (cp_name, "FieldRef") ||
	!strcmp (cp_name, "InterfaceMethodRef")) {
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
		str = r_str_newf ("%s/%s%s%s", class_str, name_str,
			space_bn_name_type ? " " : "", desc_str);
		if (class_str != empty) {
			free (class_str);
		}
		if (name_str != empty) {
			free (name_str);
		}
		if (desc_str != empty) {
			free (desc_str);
		}
	} else if (!strcmp (cp_name, "String")) {
		string_str = r_bin_java_get_utf8_from_bin_cp_list (BIN_OBJ, item->info.cp_string.string_idx);
		str = NULL;
		IFDBG eprintf("java_resolve String got: (%d) %s\n", item->info.cp_string.string_idx, string_str);
		if (!string_str) {
			string_str = empty;
		}
		str = r_str_newf ("\"%s\"", string_str);
		IFDBG eprintf("java_resolve String return: %s\n", str);
		if (string_str != empty) {
			free (string_str);
		}

	} else if (!strcmp (cp_name, "Utf8")) {
		char *tmp_str = convert_string ((const char *) item->info.cp_utf8.bytes, item->info.cp_utf8.length);
		ut32 tmp_str_len = tmp_str ? strlen (tmp_str) + 4 : 0;
		if (tmp_str) {
			str = malloc (tmp_str_len + 4);
			snprintf (str, tmp_str_len + 4, "\"%s\"", tmp_str);
		}
		free (tmp_str);
	} else if (!strcmp (cp_name, "Long")) {
		str = r_str_newf ("0x%"PFMT64x, r_bin_java_raw_to_long (item->info.cp_long.bytes.raw, 0));
	} else if (!strcmp (cp_name, "Double")) {
		str = r_str_newf ("%f", r_bin_java_raw_to_double (item->info.cp_double.bytes.raw, 0));
	} else if (!strcmp (cp_name, "Integer")) {
		str = r_str_newf ("0x%08x", R_BIN_JAVA_UINT (item->info.cp_integer.bytes.raw, 0));
	} else if (!strcmp (cp_name, "Float")) {
		str = r_str_newf ("%f", R_BIN_JAVA_FLOAT (item->info.cp_float.bytes.raw, 0));
	} else if (!strcmp (cp_name, "NameAndType")) {
		name_str = r_bin_java_get_item_name_from_bin_cp_list (BIN_OBJ, item);
		if (!name_str) {
			name_str = empty;
		}
		desc_str = r_bin_java_get_item_desc_from_bin_cp_list (BIN_OBJ, item);
		if (!desc_str) {
			desc_str = empty;
		}
		str = r_str_newf ("%s%s%s", name_str, space_bn_name_type ? " " : "", desc_str);
		if (name_str != empty) {
			free (name_str);
		}
		if (desc_str != empty) {
			free (desc_str);
		}
	} else {
		str = strdup ("(null)");
	}
	return str;
}