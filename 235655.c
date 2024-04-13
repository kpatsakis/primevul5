static Jsi_RC jsi_csNewCData(Jsi_Interp *interp, CDataObj *cd, int flags) {

    Jsi_StructSpec *slKey = NULL, *keySpec = NULL, *sf = cd->sf, *sl = cd->sl;
    
    if (!sf)
        cd->sf = sf = jsi_csStructFields(interp, cd->structName);
    if (!sl)
        sl = cd->sl = Jsi_CDataStruct(interp, cd->structName);
    if (!sf)
        return Jsi_LogError("missing struct/fields: %s", cd->structName);
    
    if (cd->keyName) {
        slKey = keySpec = Jsi_CDataStruct(interp, cd->keyName);
        if (slKey == NULL)
            return Jsi_LogError("unknown key struct: %s", cd->keyName);
    }

    const char *vparm = cd->varParam;
    if (vparm && vparm[0]) {
        char parm[200] = {}, *parms=parm, *ep;
        int plen = Jsi_Strlen(vparm);
        if (plen>=2 && vparm[0] == '[' && vparm[plen-1]==']') {
            snprintf(parm, sizeof(parm), "%.*s", plen-2, vparm+1);
            int sz = 0;
            if (parm[0] && isdigit(parm[0])) {
                sz=strtoul(parm, &ep, 0);
                if (*ep || sz<=0)
                    return Jsi_LogError("bad array size: %s", vparm);
                cd->arrSize = sz;
            } else {
                Jsi_EnumSpec *ei = (typeof(ei))Jsi_HashGet(interp->EnumItemHash, parm, 0);
                if (!ei || (sz=ei->value)<=0)
                    return Jsi_LogError("bad array enum: %s", vparm);
            }
            
        } else if (plen>=2 && vparm[0] == '{' && vparm[plen-1]=='}') {
            snprintf(parm, sizeof(parm), "%.*s", plen-2, vparm+1);
            cd->mapType = JSI_MAP_TREE;
            if (parms[0]) {
                const char *ktn = NULL;
                if (*parms == '#') {
                     cd->mapType = JSI_MAP_HASH;
                     parms++;
                }
                if (*parms == '0') {
                    cd->keyType = JSI_KEYS_ONEWORD;
                    if (parms[1])
                        return Jsi_LogError("Trailing junk: %s", vparm);
                } else if (parms[0] == '@') {
                    slKey = Jsi_CDataStruct(interp, ktn=(parms+1));
                    if (!slKey)
                        return Jsi_LogError("unknown key struct: %s", ktn);
                    cd->keyName = slKey->name;
                } else if (parms[0])
                        return Jsi_LogError("Trailing junk: %s", vparm);
            }

        } else
            return Jsi_LogError("expected either {} or []: %s", vparm);
    
    }
    cd->sl->value++;
 
    if (cd->keyName) {
        cd->slKey = Jsi_CDataStruct(interp, cd->keyName);
        if (!cd->slKey)
            return Jsi_LogError("unknown key struct: %s", cd->keyName);
        cd->keysf = jsi_csStructFields(interp, cd->keyName);
        cd->keyType = (Jsi_Key_Type)slKey->size;
        cd->slKey->value++;
    }
    
    if (cd->arrSize<=0 && cd->mapType != JSI_MAP_NONE) {
        if (interp->cdataNewVal && interp->cdataNewVal->data)
            cd->mapPtr = (Jsi_Map**)(interp->cdataNewVal->data);
        else
            cd->mapPtr = (Jsi_Map**)&cd->data;
        cd->isAlloc = 1;
        *cd->mapPtr = Jsi_MapNew(interp, cd->mapType, cd->keyType, jsi_csMapFree);
        if (cd->slKey) {
            Jsi_MapOpts mo;
            Jsi_MapConf(*cd->mapPtr, &mo, 0);
            mo.fmtKeyProc = jsi_csFmtKeyCmd;
            mo.user = (void*)cd;
            Jsi_MapConf(*cd->mapPtr, &mo, 1);
        }
    } else {
        uint i, sz = (cd->arrSize<=0 ? 1 : cd->arrSize);
        cd->keyType = JSI_KEYS_ONEWORD;
        if (interp->cdataNewVal && interp->cdataNewVal->data)
            cd->data = interp->cdataNewVal->data;
        else {
            cd->isAlloc = 1;
            cd->data = (typeof(cd->data))Jsi_Calloc(sz, cd->sl->size);
        }
        for (i=0; i<sz; i++)
            jsi_csStructInit(sl, (((uchar*)cd->data) + i*cd->sl->size));
    }

    return JSI_OK;
}