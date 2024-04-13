int do_show(FILE *stream, const char *path_p, const struct stat *st,
            acl_t acl, acl_t dacl)
{
	struct name_list *acl_names = get_list(st, acl),
	                 *first_acl_name = acl_names;
	struct name_list *dacl_names = get_list(st, dacl),
	                 *first_dacl_name = dacl_names;
	
	int acl_names_width = max_name_length(acl_names);
	int dacl_names_width = max_name_length(dacl_names);
	acl_entry_t acl_ent;
	acl_entry_t dacl_ent;
	char acl_mask[ACL_PERMS+1], dacl_mask[ACL_PERMS+1];
	int ret;

	names_width = 8;
	if (acl_names_width > names_width)
		names_width = acl_names_width;
	if (dacl_names_width > names_width)
		names_width = dacl_names_width;

	acl_mask[0] = '\0';
	if (acl) {
		acl_mask_perm_str(acl, acl_mask);
		ret = acl_get_entry(acl, ACL_FIRST_ENTRY, &acl_ent);
		if (ret == 0)
			acl = NULL;
		if (ret < 0)
			return ret;
	}
	dacl_mask[0] = '\0';
	if (dacl) {
		acl_mask_perm_str(dacl, dacl_mask);
		ret = acl_get_entry(dacl, ACL_FIRST_ENTRY, &dacl_ent);
		if (ret == 0)
			dacl = NULL;
		if (ret < 0)
			return ret;
	}
	fprintf(stream, "# file: %s\n", xquote(path_p, "\n\r"));
	while (acl_names != NULL || dacl_names != NULL) {
		acl_tag_t acl_tag, dacl_tag;

		if (acl)
			acl_get_tag_type(acl_ent, &acl_tag);
		if (dacl)
			acl_get_tag_type(dacl_ent, &dacl_tag);

		if (acl && (!dacl || acl_tag < dacl_tag)) {
			show_line(stream, &acl_names, acl, &acl_ent, acl_mask,
			          NULL, NULL, NULL, NULL);
			continue;
		} else if (dacl && (!acl || dacl_tag < acl_tag)) {
			show_line(stream, NULL, NULL, NULL, NULL,
			          &dacl_names, dacl, &dacl_ent, dacl_mask);
			continue;
		} else {
			if (acl_tag == ACL_USER || acl_tag == ACL_GROUP) {
				id_t  *acl_id_p = NULL, *dacl_id_p = NULL;
				if (acl_ent)
					acl_id_p = acl_get_qualifier(acl_ent);
				if (dacl_ent)
					dacl_id_p = acl_get_qualifier(dacl_ent);
				
				if (acl && (!dacl || *acl_id_p < *dacl_id_p)) {
					show_line(stream, &acl_names, acl,
					          &acl_ent, acl_mask,
						  NULL, NULL, NULL, NULL);
					continue;
				} else if (dacl &&
					(!acl || *dacl_id_p < *acl_id_p)) {
					show_line(stream, NULL, NULL, NULL,
					          NULL, &dacl_names, dacl,
						  &dacl_ent, dacl_mask);
					continue;
				}
			}
			show_line(stream, &acl_names,  acl,  &acl_ent, acl_mask,
				  &dacl_names, dacl, &dacl_ent, dacl_mask);
		}
	}

	free_list(first_acl_name);
	free_list(first_dacl_name);

	return 0;
}