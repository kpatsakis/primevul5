static Jsi_RC CDataInfoCmd(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
                              Jsi_Value **ret, Jsi_Func *funcPtr)
{
    UdcGet(cd, _this, funcPtr);
    Jsi_StructSpec *sl = cd->sl;
    Jsi_DString dStr= {};
    const char *sptr = Jsi_DSPrintf(&dStr, "{struct:\"%s\", label:\"%s\"}", sl->name, cd->help?cd->help:"");
    Jsi_RC rc = JSI_ERROR;
    if (!sptr)
        return Jsi_LogError("format failed");
    else
        rc = Jsi_JSONParse(interp, sptr, ret, 0);
    Jsi_DSFree(&dStr);
    if (rc != JSI_OK)
        return rc;
    Jsi_Obj *sobj;
    Jsi_Value *svalue;
    if (cd->sf) {
        sobj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
        svalue = Jsi_ValueMakeObject(interp, NULL, sobj);
        jsi_DumpOptionSpecs(interp, sobj,(Jsi_OptionSpec*) cd->sf);
        sobj = (*ret)->d.obj;
        Jsi_ObjInsert(interp, sobj, "spec", svalue, 0);
    }
    if (cd->slKey) {
        sobj = Jsi_ObjNewType(interp, JSI_OT_ARRAY);
        svalue = Jsi_ValueMakeObject(interp, NULL, sobj);
        jsi_DumpOptionSpecs(interp, sobj, (Jsi_OptionSpec*)cd->slKey);
        sobj = (*ret)->d.obj;
        Jsi_ObjInsert(interp, sobj, "keySpec", svalue, 0);
    }    return JSI_OK;
}