static avifBool isAlphaURN(const char * urn)
{
    return !strcmp(urn, URN_ALPHA0) || !strcmp(urn, URN_ALPHA1);
}