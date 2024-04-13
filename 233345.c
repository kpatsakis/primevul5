void LibRaw::identify_finetune_by_filesize(int fsize)
{

	if (fsize == 4771840)
	{ // hack Nikon 3mpix: E880, E885, E990, E995;
	  // Olympus C-3030Z
		if (!timestamp && nikon_e995())
			strcpy(model, "E995");
	}
	else if (fsize == 2940928)
	{ // hack Nikon 2mpix: E2100, E2500
		if (!timestamp && !nikon_e2100())
			strcpy(model, "E2500");
	}
	else if (fsize == 4775936)
	{ // hack Nikon 3mpix: E3100, E3200, E3500, E3700;
	  // Pentax "Optio 33WR";
	  // Olympus C-740UZ
		if (!timestamp)
			nikon_3700();
	}
	else if (fsize == 5869568)
	{ // hack Nikon 4mpix: E4300;
	  // hack Minolta "DiMAGE Z2"
		if (!timestamp && minolta_z2())
		{
			maker_index = LIBRAW_CAMERAMAKER_Minolta;
			strcpy(make, "Minolta");
			strcpy(model, "DiMAGE Z2");
		}
	}
}