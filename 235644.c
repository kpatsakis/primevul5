Jsi_RC jsi_csInitVarDefs(Jsi_Interp *interp)
{
    Jsi_VarSpec *vd = interp->statics->vars;
    int i;
    for (i=0; vd[i].name; i++) {
        SIGASSERT(vd+i, OPTS_VARDEF);
        const char *name = vd[i].name;
        const char *structName = vd[i].info;
        const char *help = vd[i].help;
        const char *varParm = (const char*)vd[i].userData;

        if (JSI_OK != jsi_csCDataNew(interp, name, structName, help, varParm, vd+i))
            return JSI_ERROR;
#if 0        
        Jsi_DString dStr;
        Jsi_DSInit(&dStr);
        Jsi_DSPrintf(&dStr, "var %s = new CData({name:\"%s\", structName:\"%s\"});", name, name, structName);
        if (help)
            Jsi_DSPrintf(&dStr, ", help:\"%s\"", help);
        /*if (vd[i].value)
            Jsi_DSPrintf(&dStr, ", arrSize:%u", (uint)vd[i].value);*/
        if (varParm)
            Jsi_DSPrintf(&dStr, ", varParam:\"%s\"", varParm);
        Jsi_DSPrintf(&dStr, "});");
        Jsi_RC rc = Jsi_EvalString(interp, Jsi_DSValue(&dStr), 0);
        Jsi_DSFree(&dStr);
        if (rc != JSI_OK)
            return JSI_ERROR;
#endif
    }
    return JSI_OK;
}