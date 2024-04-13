static void discard_receive_data(int f_in, OFF_T length)
{
	receive_data(f_in, NULL, -1, 0, NULL, -1, length);
}