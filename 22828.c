PublicMarshalAndComputeName(
			    TPMT_PUBLIC     *publicArea,    // IN: public area of an object
			    TPM2B_NAME      *name           // OUT: name of the object
			    )
{
    // Will marshal a public area into a template. This is because the internal
    // format for a TPM2B_PUBLIC is a structure and not a simple BYTE buffer.
    TPM2B_TEMPLATE       marshaled;     // this is big enough to hold a
    //  marshaled TPMT_PUBLIC
    BYTE                *buffer = (BYTE *)&marshaled.t.buffer;
    // if the nameAlg is NULL then there is no name.
    if(publicArea->nameAlg == TPM_ALG_NULL)
	name->t.size = 0;
    else
	{
	    // Marshal the public area into its canonical form
	    marshaled.t.size = TPMT_PUBLIC_Marshal(publicArea, &buffer, NULL);
	    // and compute the name
	    ObjectComputeName(marshaled.t.size, marshaled.t.buffer,
			      publicArea->nameAlg, name);
	}
    return name;
}