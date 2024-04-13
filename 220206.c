is_big_endian(void)
{
	uint16_t d = 1;

	return (archive_be16dec(&d) == 1);
}