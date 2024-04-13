Jsi_RC Jsi_CDataStructInit(Jsi_Interp *interp, uchar* data, const char *sname)
{
    Jsi_StructSpec * sl = jsi_csStructGet(interp, sname);
    if (!sl)
        return Jsi_LogError("unknown struct: %s", sname);
    return jsi_csStructInit(sl, data);
}