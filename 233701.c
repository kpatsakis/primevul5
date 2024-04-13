void acl_perm_str(acl_entry_t entry, char *str)
{
	acl_permset_t permset;
	int n;

	acl_get_permset(entry, &permset);
	for (n = 0; n < (int) ACL_PERMS; n++) {
		str[n] = (acl_get_perm(permset, acl_perm_defs[n].tag) ?
		          acl_perm_defs[n].c : '-');
	}
	str[n] = '\0';
}