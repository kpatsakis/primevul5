CryptRsaGenerateKey(
		    OBJECT              *rsaKey,            // IN/OUT: The object structure in which
		    //          the key is created.
		    RAND_STATE          *rand               // IN: if not NULL, the deterministic
		    //     RNG state
		    )
{
    UINT32               i;
    BN_PRIME(bnP); // These four declarations initialize the number to 0
    BN_PRIME(bnQ);
    BN_RSA(bnD);
    BN_RSA(bnN);
    BN_WORD(bnE);
    UINT32               e;
    int                  keySizeInBits;
    TPMT_PUBLIC         *publicArea = &rsaKey->publicArea;
    TPMT_SENSITIVE      *sensitive = &rsaKey->sensitive;
    TPM_RC               retVal = TPM_RC_NO_RESULT;
    //
    // Need to make sure that the caller did not specify an exponent that is
    // not supported
    e = publicArea->parameters.rsaDetail.exponent;
    if(e == 0)
	e = RSA_DEFAULT_PUBLIC_EXPONENT;
    if(e < 65537)
	ERROR_RETURN(TPM_RC_RANGE);
    if(e != RSA_DEFAULT_PUBLIC_EXPONENT && !IsPrimeInt(e))
	ERROR_RETURN(TPM_RC_RANGE);
    BnSetWord(bnE, e);
    // Check that e is prime
    // check for supported key size.
    keySizeInBits = publicArea->parameters.rsaDetail.keyBits;
    if(((keySizeInBits % 1024) != 0)
       || (keySizeInBits > MAX_RSA_KEY_BITS)  // this might be redundant, but...
       || (keySizeInBits == 0))
	ERROR_RETURN(TPM_RC_VALUE);
    // Set the prime size for instrumentation purposes
    INSTRUMENT_SET(PrimeIndex, PRIME_INDEX(keySizeInBits / 2));
#if SIMULATION && USE_RSA_KEY_CACHE
    if(GET_CACHED_KEY(rsaKey, rand))
	return TPM_RC_SUCCESS;
#endif
    // Make sure that key generation has been tested
    TEST(ALG_NULL_VALUE);
#if USE_OPENSSL_FUNCTIONS_RSA          // libtpms added begin
    if (rand == NULL)
        return OpenSSLCryptRsaGenerateKey(rsaKey, e, keySizeInBits);
#endif                                 // libtpms added end
    // Need to initialize the privateExponent structure
    RsaInitializeExponent(&rsaKey->privateExponent);
    // The prime is computed in P. When a new prime is found, Q is checked to
    // see if it is zero.  If so, P is copied to Q and a new P is found.
    // When both P and Q are non-zero, the modulus and
    // private exponent are computed and a trial encryption/decryption is
    // performed.  If the encrypt/decrypt fails, assume that at least one of the
    // primes is composite. Since we don't know which one, set Q to zero and start
    // over and find a new pair of primes.
    for(i = 1; (retVal != TPM_RC_SUCCESS) && (i != 100); i++)
	{
	    if(_plat__IsCanceled())
		ERROR_RETURN(TPM_RC_CANCELED);
	    BnGeneratePrimeForRSA(bnP, keySizeInBits / 2, e, rand);
	    INSTRUMENT_INC(PrimeCounts[PrimeIndex]);
	    // If this is the second prime, make sure that it differs from the
	    // first prime by at least 2^100
	    if(BnEqualZero(bnQ))
		{
		    // copy p to q and compute another prime in p
		    BnCopy(bnQ, bnP);
		    continue;
		}
	    // Make sure that the difference is at least 100 bits. Need to do it this
	    // way because the big numbers are only positive values
	    if(BnUnsignedCmp(bnP, bnQ) < 0)
		BnSub(bnD, bnQ, bnP);
	    else
		BnSub(bnD, bnP, bnQ);
	    if(BnMsb(bnD) < 100)
		continue;
	    //Form the public modulus and set the unique value
	    BnMult(bnN, bnP, bnQ);
	    BnTo2B(bnN, &publicArea->unique.rsa.b,
		   (NUMBYTES)BITS_TO_BYTES(keySizeInBits));
	    // And the  prime to the sensitive area
	    BnTo2B(bnP, &sensitive->sensitive.rsa.b,
		   (NUMBYTES)BITS_TO_BYTES(keySizeInBits) / 2);
	    // Make sure everything came out right. The MSb of the values must be
	    // one
	    if(((publicArea->unique.rsa.t.buffer[0] & 0x80) == 0)
	       || ((sensitive->sensitive.rsa.t.buffer[0] & 0x80) == 0))
		FAIL(FATAL_ERROR_INTERNAL);
	    // Make sure that we can form the private exponent values
	    if(ComputePrivateExponent(bnP, bnQ, bnE, bnN, &rsaKey->privateExponent)
	       != TRUE)
		{
		    // If ComputePrivateExponent could not find an inverse for
		    // Q, then copy P and recompute P. This might
		    // cause both to be recomputed if P is also zero
		    if(BnEqualZero(bnQ))
			BnCopy(bnQ, bnP);
		    continue;
		}
	    retVal = TPM_RC_SUCCESS;
	    // Do a trial encryption decryption if this is a signing key
	    if(IS_ATTRIBUTE(publicArea->objectAttributes, TPMA_OBJECT, sign))
		{
		    BN_RSA(temp1);
		    BN_RSA(temp2);
		    BnGenerateRandomInRange(temp1, bnN, rand);
		    // Encrypt with public exponent...
		    BnModExp(temp2, temp1, bnE, bnN);
		    // ...  then decrypt with private exponent
		    RsaPrivateKeyOp(temp2, bnN, bnP, &rsaKey->privateExponent);
		    // If the starting and ending values are not the same,
		    // start over )-;
		    if(BnUnsignedCmp(temp2, temp1) != 0)
			{
			    BnSetWord(bnQ, 0);
			    retVal = TPM_RC_NO_RESULT;
			}
		}
	}
 Exit:
    if(retVal == TPM_RC_SUCCESS)
	rsaKey->attributes.privateExp = SET;
    return retVal;
}