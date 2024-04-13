char *lookup_alias(const char *alias)
{
    const DirAlias *curr = head;

    if (aliases_up == 0) {
        return NULL;
    }
    while (curr != NULL) {
        if (strcmp(curr->alias, alias) == 0) {
            return curr->dir;
        }
        curr = curr->next;
    }
    return NULL;
}