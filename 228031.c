ungetchars(int len)
{
    if (reg_recording != 0)
    {
	delete_buff_tail(&recordbuff, len);
	last_recorded_len -= len;
    }
}