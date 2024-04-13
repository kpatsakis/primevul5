st_free_table(st_table *table)
{
    st_clear(table);
    free(table->bins);
    free(table);
}