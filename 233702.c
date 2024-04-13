struct name_list *get_list(const struct stat *st, acl_t acl)
{
	struct name_list *first = NULL, *last = NULL;
	acl_entry_t ent;
	int ret = 0;

	if (acl != NULL)
		ret = acl_get_entry(acl, ACL_FIRST_ENTRY, &ent);
	if (ret != 1)
		return NULL;
	while (ret > 0) {
		acl_tag_t e_type;
		const id_t *id_p;
		const char *name = "";
		int len;

		acl_get_tag_type(ent, &e_type);
		switch(e_type) {
			case ACL_USER_OBJ:
				name = user_name(st->st_uid, opt_numeric);
				break;

			case ACL_USER:
				id_p = acl_get_qualifier(ent);
				if (id_p != NULL)
					name = user_name(*id_p, opt_numeric);
				break;

			case ACL_GROUP_OBJ:
				name = group_name(st->st_gid, opt_numeric);
				break;

			case ACL_GROUP:
				id_p = acl_get_qualifier(ent);
				if (id_p != NULL)
					name = group_name(*id_p, opt_numeric);
				break;
		}
		name = xquote(name, "\t\n\r");
		len = strlen(name);
		if (last == NULL) {
			first = last = (struct name_list *)
				malloc(sizeof(struct name_list) + len + 1);
		} else {
			last->next = (struct name_list *)
				malloc(sizeof(struct name_list) + len + 1);
			last = last->next;
		}
		if (last == NULL) {
			free_list(first);
			return NULL;
		}
		last->next = NULL;
		strcpy(last->name, name);

		ret = acl_get_entry(acl, ACL_NEXT_ENTRY, &ent);
	}
	return first;
}