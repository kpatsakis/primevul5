R_API void U(add_method_infos_to_sdb)(RBinJavaObj * bin) {
	/*
	*** Experimental and May Change ***
	Add Mehtod information to an Array
	the key for this info variable depenedent on addr, method ordinal, etc.
	Key 1, mapping to method key:
	java.<file_offset> = <method_key>
	Key 2, basic code information
	<method_key>.code = [<addr>, <size>]
	Key 3, method description
	<method_key>.info = [<access str>, <class_name>, <name>, <signature>,]
	key 4, method meta
	<method_key>.meta = [<file_offset>, ?]
	// TODO in key 3 add <class_name>?
	e.g. <access str>.<name>.<signature>
	Note: method name not used because of collisions with operator overloading
	also take note that code offset and the method offset are not the same
	values.
	*/
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaField *fm_type;
	ut32 key_size = 255,
	value_buffer_size = 1024,
	class_name_inheap = 1;
	char *method_key = NULL,
	*method_key_value = NULL,
	*value_buffer = NULL;
	char *class_name = r_bin_java_get_this_class_name (bin);
	ut64 baddr = bin->loadaddr;
	if (!class_name) {
		class_name = "unknown";
		class_name_inheap = 0;
	}
	key_size += strlen (class_name);
	value_buffer_size += strlen (class_name);
	method_key = malloc (key_size);
	value_buffer = malloc (value_buffer_size);
	method_key_value = malloc (key_size);
	snprintf (method_key, key_size, "%s.methods", class_name);
	method_key[key_size - 1] = 0;
	r_list_foreach_safe (bin->methods_list, iter, iter_tmp, fm_type) {
		char number_buffer[80];
		ut64 file_offset = fm_type->file_offset + baddr;
		snprintf (number_buffer, sizeof (number_buffer), "0x%04"PFMT64x, file_offset);
		sdb_array_push (bin->kv, method_key, number_buffer, 0);
	}
	r_list_foreach_safe (bin->methods_list, iter, iter_tmp, fm_type) {
		ut64 code_offset = r_bin_java_get_method_code_offset (fm_type) + baddr,
		code_size = r_bin_java_get_method_code_size (fm_type),
		method_offset = fm_type->file_offset + baddr;
		// generate method specific key & value
		snprintf (method_key, key_size, "%s.0x%04"PFMT64x, class_name, code_offset);
		method_key[key_size - 1] = 0;
		snprintf (method_key_value, key_size, "%s.0x%04"PFMT64x ".method", class_name, method_offset);
		method_key_value[key_size - 1] = 0;
		IFDBG eprintf("Adding %s to sdb_array: %s\n", method_key_value, method_key);
		sdb_set (bin->kv, method_key, method_key_value, 0);
		// generate code key and values
		snprintf (method_key, key_size, "%s.code", method_key_value);
		method_key[key_size - 1] = 0;
		snprintf (value_buffer, value_buffer_size, "0x%04"PFMT64x, code_offset);
		value_buffer[value_buffer_size - 1] = 0;
		sdb_array_push (bin->kv, method_key, value_buffer, 0);
		snprintf (value_buffer, value_buffer_size, "0x%04"PFMT64x, code_size);
		value_buffer[value_buffer_size - 1] = 0;
		sdb_array_push (bin->kv, method_key, value_buffer, 0);
		// generate info key, and place values in method info array
		snprintf (method_key, key_size, "%s.info", method_key_value);
		method_key[key_size - 1] = 0;
		snprintf (value_buffer, value_buffer_size, "%s", fm_type->flags_str);
		value_buffer[value_buffer_size - 1] = 0;
		IFDBG eprintf("Adding %s to sdb_array: %s\n", value_buffer, method_key);
		sdb_array_push (bin->kv, method_key, value_buffer, 0);
		snprintf (value_buffer, value_buffer_size, "%s", fm_type->class_name);
		value_buffer[value_buffer_size - 1] = 0;
		IFDBG eprintf("Adding %s to sdb_array: %s\n", value_buffer, method_key);
		sdb_array_push (bin->kv, method_key, value_buffer, 0);
		snprintf (value_buffer, value_buffer_size, "%s", fm_type->name);
		value_buffer[value_buffer_size - 1] = 0;
		IFDBG eprintf("Adding %s to sdb_array: %s\n", value_buffer, method_key);
		sdb_array_push (bin->kv, method_key, value_buffer, 0);
		snprintf (value_buffer, value_buffer_size, "%s", fm_type->descriptor);
		value_buffer[value_buffer_size - 1] = 0;
		IFDBG eprintf("Adding %s to sdb_array: %s\n", value_buffer, method_key);
		sdb_array_push (bin->kv, method_key, value_buffer, 0);
	}
	free (method_key);
	free (method_key_value);
	free (value_buffer);
	if (class_name_inheap) {
		free (class_name);
	}
}