static int table_lookup(const struct annotate_attrib *table,
                        const char *name)
{
    for ( ; table->name ; table++) {
         if (!strcasecmp(table->name, name))
            return table->entry;
    }
    return -1;
}