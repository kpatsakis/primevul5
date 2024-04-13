void crypto_reverse(BYTE* data, int length)
{
	int i, j;
	BYTE temp;

	for (i = 0, j = length - 1; i < j; i++, j--)
	{
		temp = data[i];
		data[i] = data[j];
		data[j] = temp;
	}
}