compare_records(gconstpointer a, gconstpointer b)
{
    LrYumRepoMdRecord* yum_record = (LrYumRepoMdRecord*) a;
    char *type1 = (char *) yum_record->type;
    char *type2 = (char *) b;
    return g_strcmp0(type1, type2);
}