void print_aliases(void)
{
    const DirAlias *curr = head;

    if (aliases_up == 0) {
        addreply_noformat(502, MSG_CONF_ERR);

        return;
    }
    addreply_noformat(214, MSG_ALIASES_LIST);
    while (curr != NULL) {
        char line[MAXALIASLEN + PATH_MAX + 3U];

        snprintf(line, sizeof line, " %s %s", curr->alias, curr->dir);
        addreply_noformat(0, line);
        curr = curr->next;
    }
    addreply_noformat(214, " ");
}