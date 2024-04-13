int show_line(FILE *stream, struct name_list **acl_names,  acl_t acl,
              acl_entry_t *acl_ent, const char *acl_mask,
              struct name_list **dacl_names, acl_t dacl,
	      acl_entry_t *dacl_ent, const char *dacl_mask)
{
	acl_tag_t tag_type;
	const char *tag, *name;
	char acl_perm[ACL_PERMS+1], dacl_perm[ACL_PERMS+1];

	if (acl) {
		acl_get_tag_type(*acl_ent, &tag_type);
		name = (*acl_names)->name;
	} else {
		acl_get_tag_type(*dacl_ent, &tag_type);
		name = (*dacl_names)->name;
	}

	switch(tag_type) {
		case ACL_USER_OBJ:
			tag = "USER";
			break;
		case ACL_USER:
			tag = "user";
			break;
		case ACL_GROUP_OBJ:
			tag = "GROUP";
			break;
		case ACL_GROUP:
			tag = "group";
			break;
		case ACL_MASK:
			tag = "mask";
			break;
		case ACL_OTHER:
			tag = "other";
			break;
		default:
			return -1;
	}

	memset(acl_perm, ' ', ACL_PERMS);
	acl_perm[ACL_PERMS] = '\0';
	if (acl_ent) {
		acl_perm_str(*acl_ent, acl_perm);
		if (tag_type != ACL_USER_OBJ && tag_type != ACL_OTHER &&
		    tag_type != ACL_MASK)
			apply_mask(acl_perm, acl_mask);
	}
	memset(dacl_perm, ' ', ACL_PERMS);
	dacl_perm[ACL_PERMS] = '\0';
	if (dacl_ent) {
		acl_perm_str(*dacl_ent, dacl_perm);
		if (tag_type != ACL_USER_OBJ && tag_type != ACL_OTHER &&
		    tag_type != ACL_MASK)
			apply_mask(dacl_perm, dacl_mask);
	}

	fprintf(stream, "%-5s  %*s  %*s  %*s\n",
	        tag, -names_width, name,
	        -(int)ACL_PERMS, acl_perm,
		-(int)ACL_PERMS, dacl_perm);

	if (acl_names) {
		acl_get_entry(acl, ACL_NEXT_ENTRY, acl_ent);
		(*acl_names) = (*acl_names)->next;
	}
	if (dacl_names) {
		acl_get_entry(dacl, ACL_NEXT_ENTRY, dacl_ent);
		(*dacl_names) = (*dacl_names)->next;
	}
	return 0;
}