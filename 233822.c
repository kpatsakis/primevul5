ms_escher_read_DgContainer (MSEscherState *state, MSEscherHeader *h)
{
	return ms_escher_read_container (state, h, 0, FALSE);
}