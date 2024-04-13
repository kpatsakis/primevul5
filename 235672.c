static Jsi_RC jsi_csSetupStruct(Jsi_Interp *interp, Jsi_StructSpec *sl, Jsi_FieldSpec *sf, 
    Jsi_StructSpec* recs, int flen, Jsi_OptionTypedef** stPtr, int arrCnt) {
    bool isNew;
    int i, cnt = 0, boffset = 0;
    Jsi_HashEntry *entry, *hPtr;
    if (!(hPtr=Jsi_HashEntryNew(interp->CTypeHash, sl->name, &isNew)) || !isNew)
        return Jsi_LogError("struct is c-type: %s", sl->name);
    entry = Jsi_HashEntryNew(interp->StructHash, sl->name, &isNew);
    if (!isNew)
        return Jsi_LogError("duplicate struct: %s", sl->name);
    Jsi_FieldSpec *asf = NULL, *osf = sf;
    while (sf && sf->id != JSI_OPTION_END) {
        if (!sf->type)
            sf->type = Jsi_OptionTypeInfo(sf->id);
        if (!sf->type && sf->tname)
            sf->type = Jsi_TypeLookup(interp, sf->tname);
        int isbitset = ((sf->flags&JSI_OPT_BITSET_ENUM)!=0);
        if (sf->type && sf->type->extData && (sf->type->flags&(jsi_CTYP_ENUM|jsi_CTYP_STRUCT))) {
            // A struct sub-field or a bit field mapped to an ENUM.
            Jsi_OptionSpec *es = (typeof(es))sf->type->extData;
            es->value++;
            if ((sf->type->flags&jsi_CTYP_ENUM)) {
                if (sf->bits)
                    return Jsi_LogError("enum of bits unsupported: %s", sl->name); //TODO: get working again...
                sf->custom = (isbitset ? Jsi_Opt_SwitchBitset : Jsi_Opt_SwitchEnum);
                sf->data = (void*)es->data;
                sf->id = JSI_OPTION_CUSTOM;
            }
            else if (sf->type->flags & jsi_CTYP_STRUCT) {
                sf->custom = Jsi_Opt_SwitchSuboption;
                sf->data = es->extData;
                sf->id = JSI_OPTION_CUSTOM;
            }
        }
        if (recs) {
            if (!sf->type)
                return Jsi_LogError("unknown id");
            sf->tname = sf->type->cName;
            sf->size = (isbitset?(int)sizeof(int):sf->type->size);
            if (sf->arrSize)
                sf->size *= sf->arrSize;
            sf->idx = cnt;
            sf->boffset = boffset;
            if (sf->bits) {
                if (sf->bits>=64)
                    return Jsi_LogError("bits too large");
                boffset += sf->bits;
                sf->id = JSI_OPTION_CUSTOM;
                sf->custom=Jsi_Opt_SwitchBitfield;
                sf->init.OPT_BITS=&jsi_csBitGetSet;
            } else {
                sf->offset = (boffset+7)/8;
                boffset += sf->size*8;
            }
        } else {
            boffset += sf->size*8;
        }
        sf->extData = (uchar*)sl;
        sf++, cnt++;
    }
    sl->idx = cnt;
    if (!sl->size) 
        sl->size = (boffset+7)/8;
    if (sl->ssig)
        Jsi_HashSet(interp->SigHash, (void*)(uintptr_t)sl->ssig, sl);
    int extra = 0;
    if (flen)
        extra = sl->size + ((flen+2+arrCnt*2)*sizeof(Jsi_StructSpec));
    Jsi_OptionTypedef *st = (typeof(st))Jsi_Calloc(1, sizeof(*st) + extra);
    SIGINIT(st, TYPEDEF);
    if (!recs) 
        sf = osf;
    else {
        st->extra = (uchar*)(st+1); // Space for struct initializer.
        sf =  (typeof(sf))(st->extra + sl->size);
        memcpy(sf, recs, sizeof(*sf)*(flen+1));
        sl = sf+flen+1;
        if (arrCnt)
            asf = sl+1;
        memcpy(sl, recs+flen+1, sizeof(*sl));
        for (i=0; i<flen; i++) {
            sf[i].extData = (uchar*)sl;
            if (sf[i].id == 0 && sf[i].type)
                sf[i].id = sf[i].type->id;
            if (sf[i].arrSize) {
                asf[0] = sf[i];
                asf[1] = sf[flen];
                asf->arrSize = asf->offset = 0;
                //asf->size = asf->type->size;
                sf[i].id = JSI_OPTION_CUSTOM;
                sf[i].custom=Jsi_Opt_SwitchCArray;
                sf[i].init.OPT_CARRAY = asf;
                asf += 2;
                //sf[i].extData = 
                 //   {.sig=JSI_SIG_OPTS_FIELD, .name=sf[i].name, 
                  //  JSI_OPT_CARRAY_ITEM_(JSI_SIG_OPTS_FIELD,'+otype+', '+name+', sf[i].name, .help=sf[i].help, .flags='+fflags+rest+'),\n'
                   // JSI_OPT_END_(JSI_SIG_OPTS_FIELD,'+name+', .help="Options for array field '+name+'.'+fname+'")\n  };\n\n';
                   // JSI_OPT_CARRAY_(JSI_SIG_OPTS_FIELD,'+name+', '+fname+', "'+fdescr+'", '+fflags+', '+arnam+', '+f.asize+', "'+type+'", '+csinit+'),\n';
            }
        }
    }
    st->extData = (uchar*)sl;
    sl->extData = (uchar*)sf;
    sl->type = st;
    st->cName = sl->name;
    st->idName = "CUSTOM";
    st->id = JSI_OPTION_CUSTOM;
    st->size = sl->size;
    st->flags = jsi_CTYP_DYN_MEMORY|jsi_CTYP_STRUCT;
    Jsi_HashValueSet(entry, sl);
    Jsi_HashValueSet(hPtr, st);
    st->hPtr = hPtr;
    if (stPtr)
        *stPtr = st;
    return JSI_OK;
}