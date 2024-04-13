ms_escher_read_SpContainer (MSEscherState *state, MSEscherHeader *h)
{
	return ms_escher_read_container (state, h, 0, FALSE);
}