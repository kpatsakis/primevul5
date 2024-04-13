sbni_card_probe( unsigned long  ioaddr )
{
	unsigned char  csr0;

	csr0 = inb( ioaddr + CSR0 );
	if( csr0 != 0xff  &&  csr0 != 0x00 ) {
		csr0 &= ~EN_INT;
		if( csr0 & BU_EMP )
			csr0 |= EN_INT;
      
		if( VALID_DECODER & (1 << (csr0 >> 4)) )
			return  0;
	}
   
	return  -ENODEV;
}