static Jsi_RC CDataConstructor(Jsi_Interp *interp, Jsi_Value *args, Jsi_Value *_this,
    Jsi_Value **ret, Jsi_Func *funcPtr)
{
    Jsi_Value *that = _this;
    Jsi_Obj *nobj;
    
    if (!Jsi_FunctionIsConstructor(funcPtr)) {
        Jsi_Obj *o = Jsi_ObjNew(interp);
        Jsi_PrototypeObjSet(interp, "CData", o);
        Jsi_ValueMakeObject(interp, ret, o);
        that = *ret;
    }

    CDataObj *cd = (typeof(cd))Jsi_Calloc(1,sizeof(*cd));
    cd->interp = interp;
    Jsi_Value *val = Jsi_ValueArrayIndex(interp, args, 0);
    Jsi_vtype vtyp = Jsi_ValueTypeGet(val);
    int vlen, isNew = 0;
    const char *vstr = Jsi_ValueString(interp, val, &vlen);
    Jsi_DString dStr = {};
    Jsi_Value *ival = Jsi_ValueArrayIndex(interp, args, 1);
    if (ival && Jsi_ValueIsUndef(interp, ival))
        ival = NULL;
    else if (ival && !Jsi_ValueIsObjType(interp, ival, JSI_OT_OBJECT))
        return Jsi_LogError("arg 2: expected object or undef");
    if (vstr && vlen) {
        char ech = 0;
        const char *nstr = vstr, *cp = vstr;
        while (*cp && (isalnum(*cp) || *cp=='_')) cp++;
        if (*cp) {
            int slen = cp-vstr;
            Jsi_DString sStr = {};
            nstr = Jsi_DSAppendLen(&dStr, vstr, slen);
            if (*cp == '[')
                ech = ']';
            else if (*cp == '{')
                ech = '}';
            if (!ech || vstr[vlen-1] != ech) {
                Jsi_LogError("malformed var form is not: STRUCT, STRUCT{...}, STRUCT[...]");
                goto errout;
            }
            //cp++;
            cd->varParam = Jsi_KeyAdd(interp, Jsi_DSAppendLen(&sStr, cp, vlen-slen));
        }
        cd->structName = Jsi_KeyAdd(interp, nstr);
        val = NULL;
    } else if (vtyp != JSI_VT_OBJECT) {
        Jsi_LogError("expected string, object");
        goto errout;
    }
    if (val && Jsi_OptionsProcess(interp, CDataOptions, cd, val, 0) < 0)
        goto errout;

    if (ival && (cd->mapPtr || cd->arrSize)) {
        Jsi_LogError("init can not be used with c-array/map");
        goto errout;
     }
  
    if (JSI_OK != jsi_csNewCData(interp, cd, JSI_OPT_NO_SIG))
        goto errout;

    if (ival) {
        interp->callerErr = 1;
        Jsi_RC rc = Jsi_OptionsConf(interp, (Jsi_OptionSpec*)cd->sf, cd->data, ival, ret, 0);
        interp->callerErr = 0;
        if (rc != JSI_OK) {
            isNew = 1;
            goto errout;
        }
    }
        
    nobj = (Jsi_Obj*)Jsi_ValueGetObj(interp, that);
    cd->objId = Jsi_UserObjNew(interp, &cdataobject, nobj, cd);
    if (cd->objId<0) {
        goto errout;
    }
    cd->fobj = nobj;
    return JSI_OK;
    
errout:
    Jsi_DSFree(&dStr);
    Jsi_OptionsFree(interp, CDataOptions, cd, 0);
    if (isNew)
       jsi_csObjFree(interp, cd);
    else
        Jsi_Free(cd);
    return JSI_ERROR;

}