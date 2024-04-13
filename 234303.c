static char *mapLookup(Dt_t * nm, char *name)
{
    namev_t *objp = dtmatch(nm, name);
    if (objp)
	return objp->unique_name;
    else
	return 0;
}