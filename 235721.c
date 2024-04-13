Jsi_OptionTypedef *Jsi_TypeLookup(Jsi_Interp *interp, const char *name)
{
    int isup = 1;
    const char *cp = name;
    while (*cp && isup) {
        if (*cp != '_' && !isdigit(*cp) && !isupper(*cp)) { isup=0; break; }
        cp++;
    }
    Jsi_OptionTypedef *ptr = (typeof(ptr))Jsi_HashGet((isup?interp->TYPEHash:interp->CTypeHash), name, 0);
    if (ptr)
        SIGASSERT(ptr, TYPEDEF);
    return ptr;
}