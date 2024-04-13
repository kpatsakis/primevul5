static Jsi_RC jsi_csBitGetSet(Jsi_Interp *interp, void *vrec,  Jsi_Wide* vPtr, Jsi_OptionSpec *spec, int idx, bool isSet) {
    bool us = (spec->tname && spec->tname[0] == 'u');
    if (!us)        
        return jsi_csSBitGetSet(interp, vrec, vPtr, spec, idx, isSet);

    if (*vPtr<0)
        return JSI_ERROR;

    Jsi_UWide *valPtr = (typeof(valPtr))vPtr;
    int bits = spec->bits;
    int boffs = spec->boffset;
    if (bits<1 || bits>=64) return JSI_ERROR;
    int ofs = (boffs/8);
    int bo = (boffs%8); // 0 if byte-aligned
    int Bsz = ((bits+bo+7)/8);
    uchar *rec = (uchar*)vrec;
#ifdef __SIZEOF_INT128__
    typedef unsigned __int128 utvalType;
#else
    typedef Jsi_UWide utvalType;
#endif
    utvalType tbuf[2] = {};
    uchar sbuf[20], *bptr = (uchar*)tbuf;
    memcpy(tbuf, rec+ofs, Bsz);
    Jsi_UWide mval;
    Jsi_UWide amask = ((1LL<<(bits-1))-1LL);
    utvalType tval = 0, kval = 0, lmask;
    if (bo) { // If not byte aligned, get tval and shift
        bptr = sbuf;
        kval = tval = *(typeof(tval)*)tbuf;
        tval >>= bo;
        if (!isSet) {
            mval = (Jsi_UWide)tval;
            *(Jsi_UWide*)bptr = mval;
        }
    } else
         mval = *valPtr;
        
    if (!isSet) { // Get value.
        if (!jsi_csBitSetGet(0, bptr, bits, &mval))
            return JSI_ERROR;
        *valPtr = mval;
        return JSI_OK;
    }
    
    if (!jsi_csBitSetGet(1, bptr, bits, &mval))
        return JSI_ERROR;
    if (bo) {
        tval = (typeof(tval))mval;
        lmask=(amask<<bo);
        kval &= ~lmask;
        tval <<= bo;
        tval = (kval | tval);
        *(typeof(tval)*)tbuf = tval;
    }
    memcpy(rec+ofs, tbuf, Bsz);

    return JSI_OK;    
}