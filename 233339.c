void LibRaw::identify_finetune_pentax()
{
	if (makeIs(LIBRAW_CAMERAMAKER_Pentax) ||
		makeIs(LIBRAW_CAMERAMAKER_Samsung)) {
		if (height == 2624 &&
			width == 3936) // Pentax K10D, Samsung GX10;
		{
			height = 2616;
			width = 3896;
		}
		if (height == 3136 &&
			width == 4864) // Pentax K20D, Samsung GX20;
		{
			height = 3124;
			width = 4688;
			filters = 0x16161616;
		}
	}

	if (makeIs(LIBRAW_CAMERAMAKER_Pentax)) {
		if ((width == 4352) &&
			((unique_id == PentaxID_K_r) ||
			(unique_id == PentaxID_K_x)))
		{
			width = 4309;
			filters = 0x16161616;
		}
		if ((width >= 4960) &&
			((unique_id == PentaxID_K_5) ||
			(unique_id == PentaxID_K_5_II) ||
				(unique_id == PentaxID_K_5_II_s)))
		{
			left_margin = 10;
			width = 4950;
			filters = 0x16161616;
		}
		if ((width == 6080) && (unique_id == PentaxID_K_70))
		{
			height = 4016;
			top_margin = 32;
			width = 6020;
			left_margin = 60;
		}
		if ((width == 4736) && (unique_id == PentaxID_K_7))
		{
			height = 3122;
			width = 4684;
			filters = 0x16161616;
			top_margin = 2;
		}
		if ((width == 6080) && (unique_id == PentaxID_K_3_II))
		{
			left_margin = 4;
			width = 6040;
		}
		if ((width == 6112) && (unique_id == PentaxID_KP))
		{
			// From DNG, maybe too strict
			left_margin = 54;
			top_margin = 28;
			width = 6028;
			height = raw_height - top_margin;
		}
		if ((width == 6080) && (unique_id == PentaxID_K_3))
		{
			left_margin = 4;
			width = 6040;
		}
		if ((width == 7424) && (unique_id == PentaxID_645D))
		{
			height = 5502;
			width = 7328;
			filters = 0x61616161;
			top_margin = 29;
			left_margin = 48;
		}
	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Ricoh) &&
		(height == 3014) && (width == 4096))  // Ricoh GX200
		width = 4014;
}