static inline void crc16_update_word_(FLAC__BitReader *br, brword word)
{
	register uint32_t crc = br->read_crc16;

	for ( ; br->crc16_align < FLAC__BITS_PER_WORD ; br->crc16_align += 8) {
		uint32_t shift = FLAC__BITS_PER_WORD - 8 - br->crc16_align ;
		crc = FLAC__CRC16_UPDATE ((uint32_t) (shift < FLAC__BITS_PER_WORD ? (word >> shift) & 0xff : 0), crc);
	}

	br->read_crc16 = crc;
	br->crc16_align = 0;
}