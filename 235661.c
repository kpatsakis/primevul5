static Jsi_RC jsi_csCDataNew(Jsi_Interp *interp, const char *name, const char *structName,
    const char *help, const char *varParm, Jsi_VarSpec *data) {
    Jsi_DString dStr;
    Jsi_DSInit(&dStr);
    Jsi_DSPrintf(&dStr, "var %s = new CData({name:\"%s\", structName:\"%s\"", name, name, structName);
    if (help)
        Jsi_DSPrintf(&dStr, ", help:\"%s\"", help);
    if (varParm)
        Jsi_DSPrintf(&dStr, ", varParam:\"%s\"", varParm);
    Jsi_DSPrintf(&dStr, "});");
    interp->cdataNewVal = data;
    Jsi_RC rc = Jsi_EvalString(interp, Jsi_DSValue(&dStr), 0);
    interp->cdataNewVal = NULL;
    Jsi_DSFree(&dStr);
    return rc;
}