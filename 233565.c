calc_crc32( u32  crc,  u8  *p,  u32  len )
{
	while( len-- )
		crc = CRC32( *p++, crc );

	return  crc;
}