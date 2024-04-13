static void msusb_mspipes_free(MSUSB_PIPE_DESCRIPTOR** MsPipes, UINT32 NumberOfPipes)
{
	UINT32 pnum = 0;

	if (MsPipes)
	{
		for (pnum = 0; pnum < NumberOfPipes && MsPipes[pnum]; pnum++)
			free(MsPipes[pnum]);

		free(MsPipes);
	}
}