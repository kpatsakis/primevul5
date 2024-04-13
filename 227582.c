ComputePrivateExponent(
		       bigNum               P,             // IN: first prime (size is 1/2 of bnN)
		       bigNum               Q,             // IN: second prime (size is 1/2 of bnN)
		       bigNum               E,             // IN: the public exponent
		       bigNum               N,             // IN: the public modulus
		       privateExponent_t   *pExp           // OUT:
		       )
{
    BOOL                pOK;
    BOOL                qOK;
#if CRT_FORMAT_RSA == NO
    BN_RSA(bnPhi);
    //
    RsaInitializeExponent(pExp);
    // Get compute Phi = (p - 1)(q - 1) = pq - p - q + 1 = n - p - q + 1
    pOK = BnCopy(bnPhi, N);
    pOK = pOK && BnSub(bnPhi, bnPhi, P);
    pOK = pOK && BnSub(bnPhi, bnPhi, Q);
    pOK = pOK && BnAddWord(bnPhi, bnPhi, 1);
    // Compute the multiplicative inverse d = 1/e mod Phi
    pOK = pOK && BnModInverse((bigNum)&pExp->D, E, bnPhi);
    qOK = pOK;
#else
    BN_PRIME(temp);
    bigNum              pT;
    //
    NOT_REFERENCED(N);
    RsaInitializeExponent(pExp);
    BnCopy((bigNum)&pExp->Q, Q);
    // make p the larger value so that m2 is always less than p
    if(BnUnsignedCmp(P, Q) < 0)
	{
	    pT = P;
	    P = Q;
	    Q = pT;
	}
    //dP = (1/e) mod (p-1) = d mod (p-1)
    pOK = BnSubWord(temp, P, 1);
    pOK = pOK && BnModInverse((bigNum)&pExp->dP, E, temp);
    //dQ = (1/e) mod (q-1) = d mod (q-1)
    qOK = BnSubWord(temp, Q, 1);
    qOK = qOK && BnModInverse((bigNum)&pExp->dQ, E, temp);
    // qInv = (1/q) mod p
    if(pOK && qOK)
	pOK = qOK = BnModInverse((bigNum)&pExp->qInv, Q, P);
#endif
    if(!pOK)
	BnSetWord(P, 0);
    if(!qOK)
	BnSetWord(Q, 0);
    return pOK && qOK;
}