static namev_t *make_nitem(Dt_t * d, namev_t * objp, Dtdisc_t * disc)
{
    namev_t *np = NEW(namev_t);
    np->name = objp->name;
    np->unique_name = 0;
    return np;
}