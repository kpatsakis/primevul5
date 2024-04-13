static void addToMap(Dt_t * map, char *name, char *uniqueName)
{
    namev_t obj;
    namev_t *objp;

    obj.name = name;
    objp = dtinsert(map, &obj);
    assert(objp->unique_name == 0);
    objp->unique_name = strdup(uniqueName);
}