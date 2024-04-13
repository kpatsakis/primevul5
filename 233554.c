skip_tail( unsigned int  ioaddr,  unsigned int  tail_len,  u32 crc )
{
	while( tail_len-- )
		crc = CRC32( inb( ioaddr + DAT ), crc );

	return  crc == CRC32_REMAINDER;
}