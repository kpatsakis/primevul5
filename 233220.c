static void convert_32s16u_C1R(const OPJ_INT32* pSrc, OPJ_BYTE* pDst,
                               OPJ_SIZE_T length)
{
    OPJ_SIZE_T i;
    for (i = 0; i < length; i++) {
        OPJ_UINT32 val = (OPJ_UINT32)pSrc[i];
        *pDst++ = (OPJ_BYTE)(val >> 8);
        *pDst++ = (OPJ_BYTE)val;
    }
}