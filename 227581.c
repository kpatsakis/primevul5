RsaPrivateKeyOp(
		bigNum               inOut, // IN/OUT: number to be exponentiated
		bigNum               N,     // IN: public modulus (can be NULL if CRT)
		bigNum               P,     // IN: one of the primes (can be NULL if not CRT)
		privateExponent_t   *pExp
		)
{
    BOOL                 OK;
#if CRT_FORMAT_RSA == NO
    (P);
    OK = BnModExp(inOut, inOut, (bigNum)&pExp->D, N);
#else
    BN_RSA(M1);
    BN_RSA(M2);
    BN_RSA(M);
    BN_RSA(H);
    bigNum              Q = (bigNum)&pExp->Q;
    NOT_REFERENCED(N);
    // Make P the larger prime.
    // NOTE that when the CRT form of the private key is created, dP will always
    // be computed using the larger of p and q so the only thing needed here is that
    // the primes be selected so that they agree with dP.
    if(BnUnsignedCmp(P, Q) < 0)
	{
	    bigNum      T = P;
	    P = Q;
	    Q = T;
	}
    // m1 = cdP mod p
    OK = BnModExp(M1, inOut, (bigNum)&pExp->dP, P);
    // m2 = cdQ mod q
    OK = OK && BnModExp(M2, inOut, (bigNum)&pExp->dQ, Q);
    // h = qInv * (m1 - m2) mod p = qInv * (m1 + P - m2) mod P because Q < P
    // so m2 < P
    OK = OK && BnSub(H, P, M2);
    OK = OK && BnAdd(H, H, M1);
    OK = OK && BnModMult(H, H, (bigNum)&pExp->qInv, P);
    // m = m2 + h * q
    OK = OK && BnMult(M, H, Q);
    OK = OK && BnAdd(inOut, M2, M);
#endif
    return OK;
}