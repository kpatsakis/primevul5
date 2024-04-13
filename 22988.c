R_API void U(add_cp_objs_to_sdb)(RBinJavaObj * bin) {
	/*
	Add Constant Pool Serialized Object to an Array
	the key for this info is:
	Key:
	java.<classname>.cp_obj
	Each Value varies by type:
	In general its:
	<ordinal>.<file_offset>.<type_name>.[type specific stuff]
	Example:
	UTF-8:  <ordinal>.<file_offset>.<type_name>.<strlen>.<hexlified(str)>
	Integer: <ordinal>.<file_offset>.<type_name>.<abs(int)>
	Long: <ordinal>.<file_offset>.<type_name>.abs(long)>
	FieldRef/MethodRef: <ordinal>.<file_offset>.<type_name>.<class_idx>.<name_and_type_idx>
	*/
	ut32 idx = 0, class_name_inheap = 1;
	RBinJavaCPTypeObj *cp_obj = NULL;
	char *key = NULL,
	*value = NULL;
	char str_cnt[40];
	char *class_name = r_bin_java_get_this_class_name (bin);
	ut32 key_buf_size = 0;
	if (!class_name) {
		class_name = "unknown";
		class_name_inheap = 0;
	}
	// 4 - format, 8 number, 1 null byte, 7 "unknown"
	key_buf_size = strlen (class_name) + 4 + 8 + 1;
	key = malloc (key_buf_size);
	if (key == NULL) {
		if (class_name_inheap) {
			free (class_name);
		}
		return;
	}
	snprintf (key, key_buf_size - 1, "%s.cp_count", class_name);
	key[key_buf_size - 1] = 0;
	snprintf (str_cnt, 39, "%d", bin->cp_count);
	str_cnt[39] = 0;
	sdb_set (bin->kv, key, value, 0);
	// sdb_alist(bin->kv, key);
	for (idx = 0; idx < bin->cp_count; idx++) {
		snprintf (key, key_buf_size - 1, "%s.cp.%d", class_name, idx);
		key[key_buf_size - 1] = 0;
		cp_obj = (RBinJavaCPTypeObj *) r_bin_java_get_item_from_bin_cp_list (bin, idx);
		IFDBG eprintf("Adding %s to the sdb.\n", key);
		if (cp_obj) {
			value = ((RBinJavaCPTypeMetas *)
			cp_obj->metas->type_info)->
			allocs->stringify_obj (cp_obj);
			sdb_set (bin->kv, key, value, 0);
			free (value);
		}
	}
	if (class_name_inheap) {
		free (class_name);
	}
	free (key);
}