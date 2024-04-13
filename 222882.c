serialize_list(const char *varname, struct list_members *members)
{
    struct list_member *lm, *next;
    size_t len, result_size;
    char *result;
    debug_decl(serialize_list, SUDOERS_DEBUG_PLUGIN);

    result_size = strlen(varname) + 1;
    SLIST_FOREACH(lm, members, entries) {
	result_size += strlen(lm->value) + 1;
    }
    if ((result = malloc(result_size)) == NULL)
	goto bad;
    /* No need to check len for overflow here. */
    len = strlcpy(result, varname, result_size);
    result[len++] = '=';
    result[len] = '\0';
    SLIST_FOREACH_SAFE(lm, members, entries, next) {
	len = strlcat(result, lm->value, result_size);
	if (len + (next != NULL) >= result_size) {
	    sudo_warnx(U_("internal error, %s overflow"), __func__);
	    goto bad;
	}
	if (next != NULL) {
	    result[len++] = ',';
	    result[len] = '\0';
	}
    }
    debug_return_str(result);
bad:
    free(result);
    debug_return_str(NULL);
}