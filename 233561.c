check_fhdr( u32  ioaddr,  u32  *framelen,  u32  *frameno,  u32  *ack,
	    u32  *is_first,  u32  *crc_p )
{
	u32  crc = *crc_p;
	u8   value;

	if( inb( ioaddr + DAT ) != SBNI_SIG )
		return  0;

	value = inb( ioaddr + DAT );
	*framelen = (u32)value;
	crc = CRC32( value, crc );
	value = inb( ioaddr + DAT );
	*framelen |= ((u32)value) << 8;
	crc = CRC32( value, crc );

	*ack = *framelen & FRAME_ACK_MASK;
	*is_first = (*framelen & FRAME_FIRST) != 0;

	if( (*framelen &= FRAME_LEN_MASK) < 6
	    ||  *framelen > SBNI_MAX_FRAME - 3 )
		return  0;

	value = inb( ioaddr + DAT );
	*frameno = (u32)value;
	crc = CRC32( value, crc );

	crc = CRC32( inb( ioaddr + DAT ), crc );	/* reserved byte */
	*framelen -= 2;

	*crc_p = crc;
	return  1;
}