RSAEP(
      TPM2B       *dInOut,        // IN: size of the encrypted block and the size of
      //     the encrypted value. It must be the size of
      //     the modulus.
      // OUT: the encrypted data. Will receive the
      //      decrypted value
      OBJECT      *key            // IN: the key to use
      )
{
    TPM2B_TYPE(4BYTES, 4);
    TPM2B_4BYTES(e) = {{4, {(BYTE)((RSA_DEFAULT_PUBLIC_EXPONENT >> 24) & 0xff),
			    (BYTE)((RSA_DEFAULT_PUBLIC_EXPONENT >> 16) & 0xff),
			    (BYTE)((RSA_DEFAULT_PUBLIC_EXPONENT >> 8) & 0xff),
			    (BYTE)((RSA_DEFAULT_PUBLIC_EXPONENT)& 0xff)}}};
    //
    if(key->publicArea.parameters.rsaDetail.exponent != 0)
	UINT32_TO_BYTE_ARRAY(key->publicArea.parameters.rsaDetail.exponent,
			     e.t.buffer);
    return ModExpB(dInOut->size, dInOut->buffer, dInOut->size, dInOut->buffer,
		   e.t.size, e.t.buffer, key->publicArea.unique.rsa.t.size,
		   key->publicArea.unique.rsa.t.buffer);
}