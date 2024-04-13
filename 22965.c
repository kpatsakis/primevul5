R_API void U(add_field_infos_to_sdb)(RBinJavaObj * bin) {
	/*
	*** Experimental and May Change ***
	Add field information to an Array
	the key for this info variable depenedent on addr, method ordinal, etc.
	Key 1, mapping to method key:
	java.<file_offset> = <field_key>
	Key 3, method description
	<field_key>.info = [<access str>, <class_name>, <name>, <signature>]
	key 4, method meta
	<field_key>.meta = [<file_offset>, ?]
	*/
	RListIter *iter = NULL, *iter_tmp = NULL;
	RBinJavaField *fm_type;
	ut32 key_size = 255,
	value_buffer_size = 1024,
	class_name_inheap = 1;
	char *field_key = NULL,
	*field_key_value = NULL,
	*value_buffer = NULL;
	char *class_name = r_bin_java_get_this_class_name (bin);
	if (!class_name) {
		class_name = "unknown";
		class_name_inheap = 0;
	}
	key_size += strlen (class_name);
	value_buffer_size += strlen (class_name);
	field_key = malloc (key_size);
	value_buffer = malloc (value_buffer_size);
	field_key_value = malloc (key_size);
	snprintf (field_key, key_size, "%s.methods", class_name);
	field_key[key_size - 1] = 0;
	r_list_foreach_safe (bin->fields_list, iter, iter_tmp, fm_type) {
		char number_buffer[80];
		ut64 file_offset = fm_type->file_offset + bin->loadaddr;
		snprintf (number_buffer, sizeof (number_buffer), "0x%04"PFMT64x, file_offset);
		IFDBG eprintf("Inserting: []%s = %s\n", field_key, number_buffer);
		sdb_array_push (bin->kv, field_key, number_buffer, 0);
	}
	r_list_foreach_safe (bin->fields_list, iter, iter_tmp, fm_type) {
		ut64 field_offset = fm_type->file_offset + bin->loadaddr;
		// generate method specific key & value
		snprintf (field_key, key_size, "%s.0x%04"PFMT64x, class_name, field_offset);
		field_key[key_size - 1] = 0;
		snprintf (field_key_value, key_size, "%s.0x%04"PFMT64x ".field", class_name, field_offset);
		field_key_value[key_size - 1] = 0;
		sdb_set (bin->kv, field_key, field_key_value, 0);
		IFDBG eprintf("Inserting: %s = %s\n", field_key, field_key_value);
		// generate info key, and place values in method info array
		snprintf (field_key, key_size, "%s.info", field_key_value);
		field_key[key_size - 1] = 0;
		snprintf (value_buffer, value_buffer_size, "%s", fm_type->flags_str);
		value_buffer[value_buffer_size - 1] = 0;
		sdb_array_push (bin->kv, field_key, value_buffer, 0);
		IFDBG eprintf("Inserting: []%s = %s\n", field_key, value_buffer);
		snprintf (value_buffer, value_buffer_size, "%s", fm_type->class_name);
		value_buffer[value_buffer_size - 1] = 0;
		sdb_array_push (bin->kv, field_key, value_buffer, 0);
		IFDBG eprintf("Inserting: []%s = %s\n", field_key, value_buffer);
		snprintf (value_buffer, value_buffer_size, "%s", fm_type->name);
		value_buffer[value_buffer_size - 1] = 0;
		sdb_array_push (bin->kv, field_key, value_buffer, 0);
		IFDBG eprintf("Inserting: []%s = %s\n", field_key, value_buffer);
		snprintf (value_buffer, value_buffer_size, "%s", fm_type->descriptor);
		value_buffer[value_buffer_size - 1] = 0;
		sdb_array_push (bin->kv, field_key, value_buffer, 0);
		IFDBG eprintf("Inserting: []%s = %s\n", field_key, value_buffer);
	}
	free (field_key);
	free (field_key_value);
	free (value_buffer);
	if (class_name_inheap) {
		free (class_name);
	}
}