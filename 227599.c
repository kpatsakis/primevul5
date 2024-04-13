RSAES_PKCS1v1_5Encode(
		      TPM2B       *padded,        // OUT: the pad data
		      TPM2B       *message,       // IN: the message being padded
		      RAND_STATE  *rand
		      )
{
    UINT32      ps = padded->size - message->size - 3;
    //
    if(message->size > padded->size - 11)
	return TPM_RC_VALUE;
    // move the message to the end of the buffer
    memcpy(&padded->buffer[padded->size - message->size], message->buffer,
	   message->size);
    // Set the first byte to 0x00 and the second to 0x02
    padded->buffer[0] = 0;
    padded->buffer[1] = 2;
    // Fill with random bytes
    DRBG_Generate(rand, &padded->buffer[2], (UINT16)ps);
    // Set the delimiter for the random field to 0
    padded->buffer[2 + ps] = 0;
    // Now, the only messy part. Make sure that all the 'ps' bytes are non-zero
    // In this implementation, use the value of the current index
    for(ps++; ps > 1; ps--)
	{
	    if(padded->buffer[ps] == 0)
		padded->buffer[ps] = 0x55;  // In the < 0.5% of the cases that the
	    // random value is 0, just pick a value to
	    // put into the spot.
	}
    return TPM_RC_SUCCESS;
}