RsaInitializeExponent(
		      privateExponent_t      *pExp
		      )
{
#if CRT_FORMAT_RSA == NO
    BN_INIT(pExp->D);
#else
    BN_INIT(pExp->Q);
    BN_INIT(pExp->dP);
    BN_INIT(pExp->dQ);
    BN_INIT(pExp->qInv);
#endif
}