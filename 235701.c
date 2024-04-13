static void jsi_csInitStructTables(Jsi_Interp *interp)
{
    Jsi_StructSpec *sf, *sl = interp->statics->structs;
    while (sl  && sl->name) {
        sf = (typeof(sf))sl->data;
        jsi_csSetupStruct(interp, sl, sf, NULL, 0, NULL, 0);
        sl++;
    }
}