Jsi_OptionSpec *jsi_csgetEnum(Jsi_Interp *interp, const char *name)
{
    Jsi_HashEntry *entry = jsi_csfindInHash(interp, interp->EnumHash, name);
    return entry ? (Jsi_OptionSpec *) Jsi_HashValueGet(entry) : 0;
}