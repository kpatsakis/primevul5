Jsi_RC jsi_InitCData(Jsi_Interp *interp, int release)
{
    if (release) return jsi_DoneCData(interp);
#if JSI_USE_STUBS
    if (Jsi_StubsInit(interp, 0) != JSI_OK)
        return JSI_ERROR;
#endif

    Jsi_Hash *fsys = Jsi_UserObjRegister(interp, &cdataobject);
    if (!fsys)
        return Jsi_LogBug("Can not init cdata");

    interp->SigHash      = Jsi_HashNew(interp, JSI_KEYS_ONEWORD, NULL);
    interp->StructHash   = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);
    interp->EnumHash     = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);
    interp->EnumItemHash = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);
    interp->CTypeHash    = Jsi_HashNew(interp, JSI_KEYS_STRING, jsi_csTypeFree);
    interp->TYPEHash     = Jsi_HashNew(interp, JSI_KEYS_STRING, NULL);

    Jsi_CommandCreateSpecs(interp, cdataobject.name,  cdataCmds,  fsys, JSI_CMDSPEC_ISOBJ);
    Jsi_CommandCreateSpecs(interp, "CEnum",  enumCmds,   NULL, 0);
    Jsi_CommandCreateSpecs(interp, "CStruct",structCmds, NULL, 0);
    Jsi_CommandCreateSpecs(interp, "CType",  typeCmds,   NULL, 0);

    if (Jsi_PkgProvide(interp, cdataobject.name, 1, jsi_InitCData) != JSI_OK)
        return JSI_ERROR;
    return JSI_OK;
}