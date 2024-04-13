R_API RBinJavaField *r_bin_java_read_next_field(RBinJavaObj *bin, const ut64 offset, const ut8 *buffer, const ut64 len) {
	RBinJavaAttrInfo *attr;
	ut32 i, idx;
	ut8 buf[8];
	RBinJavaCPTypeObj *item = NULL;
	const ut8 *f_buf = buffer + offset;
	ut64 adv = 0;
	if (!bin || offset + 8 >= len) {
		return NULL;
	}
	RBinJavaField *field = (RBinJavaField *) R_NEW0 (RBinJavaField);
	if (!field) {
		eprintf ("Unable to allocate memory for field information\n");
		return NULL;
	}
	field->metas = (RBinJavaMetaInfo *) R_NEW0 (RBinJavaMetaInfo);
	if (!field->metas) {
		eprintf ("Unable to allocate memory for meta information\n");
		free (field);
		return NULL;
	}
	memcpy (buf, f_buf, 8);
	field->file_offset = offset;
	field->flags = R_BIN_JAVA_USHORT (buf, 0);
	field->flags_str = retrieve_field_access_string (field->flags);
	field->name_idx = R_BIN_JAVA_USHORT (buf, 2);
	field->descriptor_idx = R_BIN_JAVA_USHORT (buf, 4);
	field->attr_count = R_BIN_JAVA_USHORT (buf, 6);
	field->attributes = r_list_newf (r_bin_java_attribute_free);
	field->type = R_BIN_JAVA_FIELD_TYPE_FIELD;
	adv += 8;
	field->metas->ord = bin->field_idx;

	idx = field->name_idx;
	item = r_bin_java_get_item_from_bin_cp_list (bin, idx);
	field->name = r_bin_java_get_utf8_from_bin_cp_list (bin, (ut32) (field->name_idx));
	IFDBG eprintf("Field name_idx: %d, which is: ord: %d, name: %s, value: %s\n", idx, item->metas->ord, ((RBinJavaCPTypeMetas *)item->metas->type_info)->name, field->name);
	if (!field->name) {
		field->name = (char *) malloc (21);
		snprintf ((char *) field->name, 20, "sym.field_%08x", field->metas->ord);
		IFDBG eprintf("r_bin_java_read_next_field: Unable to find the name for 0x%02x index.\n", field->name_idx);
	}
	idx = field->descriptor_idx;
	item = r_bin_java_get_item_from_bin_cp_list (bin, idx);
	field->descriptor = r_bin_java_get_utf8_from_bin_cp_list (bin, (ut32) field->descriptor_idx);
	IFDBG eprintf("Field descriptor_idx: %d, which is: ord: %d, name: %s, value: %s\n", idx, item->metas->ord, ((RBinJavaCPTypeMetas *)item->metas->type_info)->name, field->descriptor);
	if (!field->descriptor) {
		field->descriptor = r_str_dup (NULL, "NULL");
		IFDBG eprintf("r_bin_java_read_next_field: Unable to find the descriptor for 0x%02x index.\n", field->descriptor_idx);
	}
	IFDBG eprintf("Looking for a NameAndType CP with name_idx: %d descriptor_idx: %d\n", field->name_idx, field->descriptor_idx);
	field->field_ref_cp_obj = r_bin_java_find_cp_ref_info_from_name_and_type (bin, field->name_idx, field->descriptor_idx);
	if (field->field_ref_cp_obj) {
		IFDBG eprintf("Found the obj.\n");
		item = r_bin_java_get_item_from_bin_cp_list (bin, field->field_ref_cp_obj->info.cp_field.class_idx);
		IFDBG eprintf("Field class reference value: %d, which is: ord: %d, name: %s\n", field->field_ref_cp_obj->info.cp_field.class_idx, item->metas->ord, ((RBinJavaCPTypeMetas *)item->metas->type_info)->name);
		field->class_name = r_bin_java_get_item_name_from_bin_cp_list (bin, item);
		IFDBG eprintf("Field requesting ref_cp_obj the following which is: ord: %d, name: %s\n", field->field_ref_cp_obj->metas->ord, ((RBinJavaCPTypeMetas *)field->field_ref_cp_obj->metas->type_info)->name);
		IFDBG eprintf("FieldRef class name resolves to: %s\n", field->class_name);
		if (!field->class_name) {
			field->class_name = r_str_dup (NULL, "NULL");
		}
	} else {
		// XXX - default to this class?
		field->field_ref_cp_obj = r_bin_java_get_item_from_bin_cp_list (bin, bin->cf2.this_class);
		field->class_name = r_bin_java_get_item_name_from_bin_cp_list (bin, field->field_ref_cp_obj);
	}
	IFDBG eprintf("Parsing %s(%s)", field->name, field->descriptor);
	if (field->attr_count > 0) {
		field->attr_offset = adv + offset;
		for (i = 0; i < field->attr_count && offset + adv < len; i++) {
			attr = r_bin_java_read_next_attr (bin, offset + adv, buffer, len);
			if (!attr) {
				eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Field Attribute: %d.\n", i);
				free (field->metas);
				free (field);
				return NULL;
			}
			if ((r_bin_java_get_attr_type_by_name (attr->name))->type == R_BIN_JAVA_ATTR_TYPE_CODE_ATTR) {
				// This is necessary for determing the appropriate number of bytes when readin
				// uoffset, ustack, ulocalvar values
				bin->cur_method_code_length = attr->info.code_attr.code_length;
				bin->offset_sz = 2;// (attr->info.code_attr.code_length > 65535) ? 4 : 2;
				bin->ustack_sz = 2;// (attr->info.code_attr.max_stack > 65535) ? 4 : 2;
				bin->ulocalvar_sz = 2;// (attr->info.code_attr.max_locals > 65535) ? 4 : 2;
			}
			r_list_append (field->attributes, attr);
			adv += attr->size;
			if (adv + offset >= len) {
				eprintf ("[X] r_bin_java: Error unable to parse remainder of classfile after Field Attribute: %d.\n", i);
				r_bin_java_fmtype_free (field);
				return NULL;
			}
		}
	}
	field->size = adv;
	return field;
}