ObjectComputeName(
		  UINT32           size,          // IN: the size of the area to digest
		  BYTE            *publicArea,    // IN: the public area to digest
		  TPM_ALG_ID       nameAlg,       // IN: the hash algorithm to use
		  TPM2B_NAME      *name           // OUT: Computed name
		  )
{
    // Hash the publicArea into the name buffer leaving room for the nameAlg
    name->t.size = CryptHashBlock(nameAlg, size, publicArea,
				  sizeof(name->t.name) - 2,
				  &name->t.name[2]);
    // set the nameAlg
    UINT16_TO_BYTE_ARRAY(nameAlg, name->t.name);
    name->t.size += 2;
    return name;
}