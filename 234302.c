static void free_nitem(Dt_t * d, namev_t * np, Dtdisc_t * disc)
{
    free(np->unique_name);
    free(np);
}