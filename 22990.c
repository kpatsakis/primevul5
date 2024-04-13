R_API char *r_bin_java_resolve_b64_encode(RBinJavaObj *BIN_OBJ, ut16 idx) {
	RBinJavaCPTypeObj *item = NULL, *item2 = NULL;
	char *class_str = NULL,
	*name_str = NULL,
	*desc_str = NULL,
	*string_str = NULL,
	*empty = "",
	*cp_name = NULL,
	*str = NULL, *out = NULL;
	int memory_alloc = 0;
	if (BIN_OBJ && BIN_OBJ->cp_count < 1) {
		// r_bin_java_new_bin(BIN_OBJ);
		return NULL;
	}
	item = (RBinJavaCPTypeObj *) r_bin_java_get_item_from_bin_cp_list (BIN_OBJ, idx);
	if (item) {
		cp_name = ((RBinJavaCPTypeMetas *) item->metas->type_info)->name;
		IFDBG eprintf("java_resolve Resolved: (%d) %s\n", idx, cp_name);
	} else {
		return NULL;
	}
	if (!strcmp (cp_name, "Class")) {
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
		memory_alloc = strlen (class_str) + strlen (name_str) + strlen (desc_str) + 3;
		if (memory_alloc) {
			str = malloc (memory_alloc);
			if (str) {
				snprintf (str, memory_alloc, "%s%s", name_str, desc_str);
				out = r_base64_encode_dyn ((const char *) str, strlen (str));
				free (str);
				str = out;
			}
		}
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
		memory_alloc = strlen (class_str) + strlen (name_str) + strlen (desc_str) + 3;
		if (memory_alloc) {
			str = malloc (memory_alloc);
			if (str) {
				snprintf (str, memory_alloc, "%s/%s%s", class_str, name_str, desc_str);
				out = r_base64_encode_dyn ((const char *) str, strlen (str));
				free (str);
				str = out;
			}
		}
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
		str = NULL;
		IFDBG eprintf("java_resolve String got: (%d) %s\n", item->info.cp_string.string_idx, string_str);
		if (!string_str) {
			string_str = empty;
			length = strlen (empty);
		}
		memory_alloc = length + 3;
		if (memory_alloc) {
			str = malloc (memory_alloc);
			if (str) {
				snprintf (str, memory_alloc, "\"%s\"", string_str);
				out = r_base64_encode_dyn ((const char *) str, strlen (str));
				free (str);
				str = out;
			}
		}
		IFDBG eprintf("java_resolve String return: %s\n", str);
		if (string_str != empty) {
			free (string_str);
		}
	} else if (strcmp (cp_name, "Utf8") == 0) {
		ut64 sz = item->info.cp_utf8.length ? item->info.cp_utf8.length + 10 : 10;
		str = malloc (sz);
		memset (str, 0, sz);
		if (sz > 10) {
			r_base64_encode (str, item->info.cp_utf8.bytes, item->info.cp_utf8.length);
		}
	} else if (strcmp (cp_name, "Long") == 0) {
		str = malloc (34);
		if (str) {
			snprintf (str, 34, "0x%"PFMT64x, r_bin_java_raw_to_long (item->info.cp_long.bytes.raw, 0));
			out = r_base64_encode_dyn ((const char *) str, strlen (str));
			free (str);
			str = out;
		}
	} else if (strcmp (cp_name, "Double") == 0) {
		str = malloc (1000);
		if (str) {
			snprintf (str, 1000, "%f", r_bin_java_raw_to_double (item->info.cp_double.bytes.raw, 0));
			out = r_base64_encode_dyn ((const char *) str, strlen (str));
			free (str);
			str = out;
		}
	} else if (strcmp (cp_name, "Integer") == 0) {
		str = calloc (34, 1);
		if (str) {
			snprintf (str, 34, "0x%08x", R_BIN_JAVA_UINT (item->info.cp_integer.bytes.raw, 0));
			out = r_base64_encode_dyn ((const char *) str, strlen (str));
			free (str);
			str = out;
		}
	} else if (strcmp (cp_name, "Float") == 0) {
		str = malloc (34);
		if (str) {
			snprintf (str, 34, "%f", R_BIN_JAVA_FLOAT (item->info.cp_float.bytes.raw, 0));
			out = r_base64_encode_dyn ((const char *) str, strlen (str));
			free (str);
			str = out;
		}
	} else if (!strcmp (cp_name, "NameAndType")) {
		name_str = r_bin_java_get_item_name_from_bin_cp_list (BIN_OBJ, item);
		if (!name_str) {
			name_str = empty;
		}
		desc_str = r_bin_java_get_item_desc_from_bin_cp_list (BIN_OBJ, item);
		if (!desc_str) {
			desc_str = empty;
		}
		memory_alloc = strlen (name_str) + strlen (desc_str) + 3;
		if (memory_alloc) {
			str = malloc (memory_alloc);
			if (str) {
				snprintf (str, memory_alloc, "%s %s", name_str, desc_str);
				out = r_base64_encode_dyn ((const char *) str, strlen (str));
				free (str);
				str = out;
			}
		}
		if (name_str != empty) {
			free (name_str);
		}
		if (desc_str != empty) {
			free (desc_str);
		}
	} else {
		str = r_base64_encode_dyn ((const char *) "(null)", 6);
	}
	return str;
}