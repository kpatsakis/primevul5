Jsi_RC Jsi_CDataRegister(Jsi_Interp *interp, Jsi_CData_Static *statics)
{
    Jsi_RC rc = JSI_OK;
    if (statics) {
        if (interp->statics)
            statics->nextPtr = interp->statics;
        interp->statics = statics;
        jsi_csInitType(interp);
        jsi_csInitStructTables(interp);
        jsi_csInitEnum(interp);
        jsi_csInitEnumItem(interp);
        rc = jsi_csInitVarDefs(interp);
    }
    return rc;
}