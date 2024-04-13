static void convert_16u32s_C1R(const OPJ_BYTE* pSrc, OPJ_INT32* pDst,
                               OPJ_SIZE_T length)
{
    OPJ_SIZE_T i;
    for (i = 0; i < length; i++) {
        OPJ_INT32 val0 = *pSrc++;
        OPJ_INT32 val1 = *pSrc++;
        pDst[i] = val0 << 8 | val1;
    }
}