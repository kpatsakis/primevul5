static Jsi_RC jsi_csSBitGetSet(Jsi_Interp *interp, void *vrec,  Jsi_Wide* vPtr, Jsi_OptionSpec *spec, int idx, bool isSet) {
    Jsi_Wide *valPtr = (typeof(valPtr))vPtr;
    int bits = spec->bits;
    int boffs = spec->boffset;
    if (bits<1 || bits>=64) return JSI_ERROR;
    int ofs = (boffs/8);
    int bo = (boffs%8); // 0 if byte-aligned
    int Bsz = ((bits+bo+7)/8);
    uchar *rec = (uchar*)vrec;
#ifdef __SIZEOF_INT128__
    typedef __int128 stvalType;
#else
    typedef Jsi_Wide stvalType;
#endif
    stvalType tbuf[2] = {};
    uchar sbuf[20], *bptr = (uchar*)tbuf;
    memcpy(tbuf, rec+ofs, Bsz);
    Jsi_Wide mval = *valPtr;
    Jsi_Wide amask = ((1LL<<(bits-1))-1LL);
    stvalType tval = 0, kval = 0, lmask;
    if (bo) { // If not byte aligned, get tval and shift
        bptr = sbuf;
        kval = tval = *(typeof(tval)*)tbuf;
        tval >>= bo;
        if (!isSet) {
            mval = (Jsi_Wide)tval;
            *(Jsi_Wide*)bptr = mval;
        }
    }
        
    if (!isSet) { // Get value.
        if (!jsi_csSBitSetGet(0, bptr, bits, &mval))
            return JSI_ERROR;
        *valPtr = mval;
        return JSI_OK;
    }
    
    if (!jsi_csSBitSetGet(1, bptr, bits, &mval))
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