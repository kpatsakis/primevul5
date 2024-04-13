void LibRaw::identify_finetune_dcr(char head[64], int fsize, int flen)
{
	static const short pana[][6] = {
		// raw_width, raw_height, left_margin, top_margin, width_increment,
		// height_increment
		{3130, 1743, 4, 0, -6, 0},      /* 00 */
		{3130, 2055, 4, 0, -6, 0},      /* 01 */
		{3130, 2319, 4, 0, -6, 0},      /* 02 DMC-FZ8 */
		{3170, 2103, 18, 0, -42, 20},   /* 03 */
		{3170, 2367, 18, 13, -42, -21}, /* 04 */
		{3177, 2367, 0, 0, -1, 0},      /* 05 DMC-L1 */
		{3304, 2458, 0, 0, -1, 0},      /* 06 DMC-FZ30 */
		{3330, 2463, 9, 0, -5, 0},      /* 07 DMC-FZ18 */
		{3330, 2479, 9, 0, -17, 4},     /* 08 */
		{3370, 1899, 15, 0, -44, 20},   /* 09 */
		{3370, 2235, 15, 0, -44, 20},   /* 10 */
		{3370, 2511, 15, 10, -44, -21}, /* 11 */
		{3690, 2751, 3, 0, -8, -3},     /* 12 DMC-FZ50 */
		{3710, 2751, 0, 0, -3, 0},      /* 13 DMC-L10 */
		{3724, 2450, 0, 0, 0, -2},      /* 14 */
		{3770, 2487, 17, 0, -44, 19},   /* 15 */
		{3770, 2799, 17, 15, -44, -19}, /* 16 */
		{3880, 2170, 6, 0, -6, 0},      /* 17 DMC-LX1 */
		{4060, 3018, 0, 0, 0, -2},      /* 18 DMC-FZ35, DMC-FZ38 */
		{4290, 2391, 3, 0, -8, -1},     /* 19 DMC-LX2 */
		{4330, 2439, 17, 15, -44, -19}, /* 20 "D-LUX 3" */
		{4508, 2962, 0, 0, -3, -4},     /* 21 */
		{4508, 3330, 0, 0, -3, -6},     /* 22 */
		{10480, 7794, 0, 0, -2, 0},     /* 23: G9 in high-res */
	};
	int i,c;
	struct jhead jh;

	if (makeIs(LIBRAW_CAMERAMAKER_Canon) && !tiff_flip && imCanon.MakernotesFlip)
	{
		tiff_flip = imCanon.MakernotesFlip;
	}

	else if (makeIs(LIBRAW_CAMERAMAKER_Nikon))
	{
		if (!load_raw)
			load_raw = &LibRaw::packed_load_raw;
		if (model[0] == 'E') // Nikon E8800, E8700, E8400, E5700, E5400, E5000,
							 // others are diag hacks?
			load_flags |= !data_offset << 2 | 2;
	}
	/* Set parameters based on camera name (for non-DNG files). */

	/* Always 512 for arw2_load_raw */
	else if (makeIs(LIBRAW_CAMERAMAKER_Sony) &&
		(raw_width > 3888) && !black && !cblack[0])
	{
		black = (load_raw == &LibRaw::sony_arw2_load_raw)
			? 512
			: (128 << (tiff_bps - 12));
	}

	if (is_foveon) {
		if (height * 2 < width)
			pixel_aspect = 0.5;
		if (height > width)
			pixel_aspect = 2;
		filters = 0;

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Pentax)) {
		if ((unique_id == PentaxID_K_1) ||
			(unique_id == PentaxID_K_1_Mark_II)) {
			top_margin = 18;
			height = raw_height - top_margin;
			if (raw_width == 7392) {
				left_margin = 6;
				width = 7376;
			}

		}
		else if (unique_id == PentaxID_Optio_S_V101) { // (fsize == 3178560)
			cam_mul[0] *= 4;
			cam_mul[2] *= 4;

		}
		else if (unique_id == PentaxID_Optio_33WR) { // (fsize == 4775936)
			flip = 1;
			filters = 0x16161616;

		}
		else if (unique_id == PentaxID_staristD) {
			load_raw = &LibRaw::unpacked_load_raw;
			data_error = -1;

		}
		else if (unique_id == PentaxID_staristDS) {
			height -= 2;
		}

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Canon)) {
		if (tiff_bps == 15) { // Canon sRAW
			if (width == 3344)
				width = 3272;
			else if (width == 3872)
				width = 3866;

			if (height > width) {
				SWAP(height, width);
				SWAP(raw_height, raw_width);
			}
			if (width == 7200 &&
				height == 3888) { // Canon EOS 5DS (R);
				raw_width = width = 6480;
				raw_height = height = 4320;
			}
			filters = 0;
			tiff_samples = colors = 3;
			load_raw = &LibRaw::canon_sraw_load_raw;
		}

		if (!strcmp(normalized_model, "PowerShot 600")) {
			height = 613;
			width = 854;
			raw_width = 896;
			colors = 4;
			filters = 0xe1e4e1e4;
			load_raw = &LibRaw::canon_600_load_raw;

		}
		else if (!strcmp(normalized_model, "PowerShot A5") ||
			!strcmp(normalized_model, "PowerShot A5 Zoom")) {
			height = 773;
			width = 960;
			raw_width = 992;
			pixel_aspect = 256 / 235.0;
			filters = 0x1e4e1e4e;
			goto canon_a5;

		}
		else if (!strcmp(normalized_model, "PowerShot A50")) {
			height = 968;
			width = 1290;
			raw_width = 1320;
			filters = 0x1b4e4b1e;
			goto canon_a5;

		}
		else if (!strcmp(normalized_model, "PowerShot Pro70")) {
			height = 1024;
			width = 1552;
			filters = 0x1e4b4e1b;
		canon_a5:
			colors = 4;
			tiff_bps = 10;
			load_raw = &LibRaw::packed_load_raw;
			load_flags = 40;

		}
		else if (!strcmp(normalized_model, "PowerShot Pro90 IS") ||
			!strcmp(normalized_model, "PowerShot G1")) {
			colors = 4;
			filters = 0xb4b4b4b4;

		}
		else if (!strcmp(normalized_model, "PowerShot A610")) { // chdk hack
			if (canon_s2is())
				strcpy(model + 10, "S2 IS"); // chdk hack

		}
		else if (!strcmp(normalized_model, "PowerShot SX220 HS")) { // chdk hack
			mask[1][3] = -4;
			top_margin = 16;
			left_margin = 92;

		}
		else if (!strcmp(normalized_model, "PowerShot S120")) { // chdk hack
			raw_width = 4192;
			raw_height = 3062;
			width = 4022;
			height = 3016;
			mask[0][0] = top_margin = 31;
			mask[0][2] = top_margin + height;
			left_margin = 120;
			mask[0][1] = 23;
			mask[0][3] = 72;

		}
		else if (!strcmp(normalized_model, "PowerShot G16")) {
			mask[0][0] = 0;
			mask[0][2] = 80;
			mask[0][1] = 0;
			mask[0][3] = 16;
			top_margin = 29;
			left_margin = 120;
			width = raw_width - left_margin - 48;
			height = raw_height - top_margin - 14;

		}
		else if (!strcmp(normalized_model, "PowerShot SX50 HS")) {
			top_margin = 17;
		}

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Nikon)) {
		if (!strcmp(model, "D1"))
		{
			imgdata.other.analogbalance[0] = cam_mul[0];
			imgdata.other.analogbalance[2] = cam_mul[2];
			imgdata.other.analogbalance[1] = imgdata.other.analogbalance[3] =
				cam_mul[1];
			cam_mul[0] = cam_mul[1] = cam_mul[2] = 1.0f;
		}

		else if (!strcmp(model, "D1X"))
		{
			width -= 4;
			pixel_aspect = 0.5;
		}
		else if (!strcmp(model, "D40X") ||
			!strcmp(model, "D60") ||
			!strcmp(model, "D80") ||
			!strcmp(model, "D3000"))
		{
			height -= 3;
			width -= 4;
		}
		else if (!strcmp(model, "D3") ||
			!strcmp(model, "D3S") ||
			!strcmp(model, "D700"))
		{
			width -= 4;
			left_margin = 2;
		}
		else if (!strcmp(model, "D3100"))
		{
			width -= 28;
			left_margin = 6;
		}
		else if (!strcmp(model, "D5000") ||
			!strcmp(model, "D90"))
		{
			width -= 42;
		}
		else if (!strcmp(model, "D5100") ||
			!strcmp(model, "D7000") ||
			!strcmp(model, "COOLPIX A"))
		{
			width -= 44;
		}
		else if (!strcmp(model, "D3200") ||
			!strcmp(model, "D600") ||
			!strcmp(model, "D610") ||
			!strncmp(model, "D800", 4)) // Nikons: D800, D800E
		{
			width -= 46;
		}
		else if (!strcmp(model, "D4") ||
			!strcmp(model, "Df"))
		{
			width -= 52;
			left_margin = 2;
		}
		else if (!strcmp(model, "D500"))
		{
			// Empty - to avoid width-1 below
		}
		else if (!strncmp(model, "D40", 3) ||
			!strncmp(model, "D50", 3) ||
			!strncmp(model, "D70", 3))
		{
			width--;
		}
		else if (!strcmp(model, "D100"))
		{
			if (load_flags) // compressed NEF
				raw_width = (width += 3) + 3;
		}
		else if (!strcmp(model, "D200"))
		{
			left_margin = 1;
			width -= 4;
			filters = 0x94949494;
		}
		else if (!strncmp(model, "D2H", 3)) // Nikons: D2H, D2Hs
		{
			left_margin = 6;
			width -= 14;
		}
		else if (!strncmp(model, "D2X", 3)) // Nikons: D2X, D2Xs
		{
			if (width == 3264) // in-camera Hi-speed crop: On
				width -= 32;
			else
				width -= 8;
		}
		else if (!strncmp(model, "D300", 4)) // Nikons: D300, D300s
		{
			width -= 32;
		}
		else if (raw_width == 4032) // Nikon "COOLPIX P7700", "COOLPIX P7800",
									// "COOLPIX P330", "COOLPIX P340"
		{
			if (!strcmp(normalized_model, "COOLPIX P7700"))
			{
				maximum = 65504;
				load_flags = 0;
			}
			else if (!strcmp(normalized_model, "COOLPIX P7800"))
			{
				maximum = 65504;
				load_flags = 0;
			}
			else if (!strcmp(model, "COOLPIX P340"))
			{
				load_flags = 0;
			}
		}
		else if (!strncmp(model, "COOLPIX P", 9) &&
			raw_width != 4032) // Nikon "COOLPIX P1000", "COOLPIX P6000",
							   // "COOLPIX P7000", "COOLPIX P7100"
		{
			load_flags = 24;
			filters = 0x94949494;
			/* the following 'if' is most probably obsolete, because we now read black
			 * level from metadata */
			if ((model[9] == '7') && /* P7000, P7100 */
				((iso_speed >= 400) || (iso_speed == 0)) &&
				!strstr(software, "V1.2")) /* v. 1.2 seen for P7000 only */
				black = 255;
		}
		else if (!strncmp(model, "COOLPIX B700", 12))
		{
			load_flags = 24;
		}
		else if (!strncmp(model, "1 ",
			2)) // Nikons: "1 AW1", "1 J1", "1 J2", "1 J3", "1 J4",
				// "1 J5", "1 S1", "1 S2", "1 V1", "1 V2", "1 V3"
		{
			height -= 2;
		}
		else if (fsize == 1581060) // hack Nikon 1mpix: E900
		{
			simple_coeff(3);
			pre_mul[0] = 1.2085;
			pre_mul[1] = 1.0943;
			pre_mul[3] = 1.1103;
		}
		else if ((fsize == 4771840) &&  // hack Nikon 3mpix: E880, E885, E990
			strcmp(model, "E995")) // but not E995
		{
			filters = 0xb4b4b4b4;
			simple_coeff(3);
			pre_mul[0] = 1.196;
			pre_mul[1] = 1.246;
			pre_mul[2] = 1.018;
		}
		else if ((fsize == 4775936) && // hack Nikon 3mpix: E3100, E3200, E3500
			(atoi(model + 1) < 3700)) // but not E3700;
		{
			filters = 0x49494949;
		}
		else if (fsize == 5869568) // hack Nikon 4mpix: E4300;
		{
			load_flags = 6;
		}
		else if (!strcmp(model, "E2500"))
		{
			height -= 2;
			load_flags = 6;
			colors = 4;
			filters = 0x4b4b4b4b;
		}
	}

	else if (makeIs(LIBRAW_CAMERAMAKER_Olympus)) {
		if (OlyID == OlyID_C_740UZ) { // (fsize == 4775936)
			i = find_green(12, 32, 1188864, 3576832);
			c = find_green(12, 32, 2383920, 2387016);
			if (abs(i) < abs(c)) {
				SWAP(i, c);
				load_flags = 24;
			}
			if (i < 0)
				filters = 0x61616161;
		}
		else if (OlyID == OlyID_C_770UZ) {
			height = 1718;
			width = 2304;
			filters = 0x16161616;
			load_raw = &LibRaw::packed_load_raw;
			load_flags = 30;
		}
		else {
			height += height & 1;
			if (exif_cfa)
				filters = exif_cfa;

			if (width == 4100) // Olympus E-PL2, E-PL1, E-P2, E-P1, E-620, E-600, E-5, E-30;
				width -= 4;

			if (width == 4080) // Olympus E-PM1, E-PL3, E-P3;
				width -= 24;

			if (width == 10400) // Olympus PEN-F, E-M1-II, E-M1-III, E-M1X
				width -= 12;

			if (width == 8200) // E-M1-III in 50Mp mode, E-M1X
				width -= 30;

			if (width == 9280) { // Olympus E-M5 Mark II;
				width -= 6;
				height -= 6;
			}

			if (load_raw == &LibRaw::unpacked_load_raw)
				load_flags = 4;
			tiff_bps = 12;
			if ((OlyID == OlyID_E_300) ||
				(OlyID == OlyID_E_500)) {
				width -= 20;
				if (load_raw == &LibRaw::unpacked_load_raw) {
					maximum = 0xfc3;
					memset(cblack, 0, sizeof cblack);
				}

			}
			else if (OlyID == OlyID_STYLUS_1) {
				width -= 16;
				maximum = 0xfff;

			}
			else if (OlyID == OlyID_E_330) {
				width -= 30;
				if (load_raw == &LibRaw::unpacked_load_raw)
					maximum = 0xf79;

			}
			else if (OlyID == OlyID_SP_550UZ) {
				thumb_length = flen - (thumb_offset = 0xa39800);
				thumb_height = 480;
				thumb_width = 640;

			}
			else if (OlyID == OlyID_TG_4) {
				width -= 16;

			}
			else if ((OlyID == OlyID_TG_5) ||
				(OlyID == OlyID_TG_6)) {
				width -= 26;
			}
		}

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_RoverShot) &&
		(fsize == 6291456)) { // RoverShot 3320AF
		fseek(ifp, 0x300000, SEEK_SET);
		if ((order = guess_byte_order(0x10000)) == 0x4d4d)
		{
			height -= (top_margin = 16);
			width -= (left_margin = 28);
			maximum = 0xf5c0;
			strcpy(make, "ISG");
			model[0] = 0;
		}

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Fujifilm)) {
		if (!strcmp(model, "S2Pro"))
		{
			height = 2144;
			width = 2880;
			flip = 6;
		}
		else if (load_raw != &LibRaw::packed_load_raw && strncmp(model, "X-", 2) &&
			filters >= 1000) // Bayer and not an X-model
			maximum = (is_raw == 2 && shot_select) ? 0x2f00 : 0x3e00;

		if (FujiCropMode == 1)
		{ // FF crop on GFX
			width = raw_width;
			height = raw_height;
		}
		else if (FujiCropMode == 4)
		{ /* electronic shutter, high speed mode (1.25x crop) */
			height = raw_height;
		}

		top_margin = (raw_height >= height) ? (raw_height - height) >> 2 << 1 : 0;
		left_margin = (raw_width >= width) ? (raw_width - width) >> 2 << 1 : 0;

		if (!strcmp(model, "X-T3") || !strcmp(model, "X-T4") || !strcmp(model, "X100V") || !strcmp(model, "X-T30") || !strcmp(model, "X-Pro3"))
		{
			top_margin = 0;
			if (FujiCropMode == 0)
			{
				top_margin = 6;
				height = 4170;
				left_margin = 0;
				width = 6246;
			}
			else if (FujiCropMode == 4)
			{ /* electronic shutter, high speed mode (1.25x crop) */
				left_margin = 624;
				width = 5004;
			}
		}

		if (width == 2848 || // Fujifilm X-S1, X10, XF1
			width == 3664)   // Fujifilm "HS10 HS11"
			filters = 0x16161616;

		if (width == 4032 || // Fujifilm X20, X30, XQ1, XQ2
			width == 4952)   // Fujifilm X-A1, X-A2, X-E1, X-M1, X-Pro1
			left_margin = 0;

		if (width == 3328 &&
			(width -= 66)) // Fujifilm F550EXR, F600EXR, F770EXR, F800EXR, F900EXR,
						   // HS20EXR, HS30EXR, HS33EXR, HS50EXR
			left_margin = 34;

		if (width == 4936) // Fujifilm X-E2S, X-E2, X-T10, X-T1, X100S, X100T, X70
			left_margin = 4;

		if (width == 6032) // Fujifilm X100F, X-T2, X-T20, X-Pro2, X-H1, X-E3
			left_margin = 0;

		if (!strcmp(normalized_model, "DBP for GX680"))
		{
			/*
			7712 2752 -> 5504 3856
			*/

			/*
			width = 688;
			height = 30848;
			raw_width = 688;
			raw_height = 30848;
			*/

			raw_width = 5504;
			raw_height = 3856;
			left_margin = 32;
			top_margin = 8;
			width = raw_width - left_margin - 32;
			height = raw_height - top_margin - 8;

			load_raw = &LibRaw::unpacked_load_raw_FujiDBP;
			//  maximum = 0x0fff;
			filters = 0x16161616;
			load_flags = 0;
			flip = 6;
		}

		if (!strcmp(model, "HS50EXR") || !strcmp(model, "F900EXR"))
		{
			width += 2;
			left_margin = 0;
			filters = 0x16161616;
		}
		if (!strncmp(model, "GFX 50", 6))
		{
			left_margin = 0;
			top_margin = 0;
		}
		if (!strncmp(model, "GFX 100", 7))
		{
			left_margin = 0;
			width = raw_width - 146;
			height = raw_height - (top_margin = 2);
			if (tiff_bps == 16)
				maximum = 0xffff;
		}
		if (!strcmp(normalized_model, "S5100"))
		{
			height -= (top_margin = 6);
		}
		if (fuji_layout)
			raw_width *= is_raw;
		if (filters == 9)
			FORC(36)
			((char *)xtrans)[c] =
			xtrans_abs[(c / 6 + top_margin) % 6][(c + left_margin) % 6];

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Konica)) {
		if (!strcmp(model, "KD-400Z")) {
			height = 1712;
			width = 2312;
			raw_width = 2336;
			goto konica_400z;
		}
		else if (!strcmp(model, "KD-510Z")) {
			goto konica_510z;
		}

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Minolta)) {
		if (fsize == 5869568) { // hack Minolta "DiMAGE Z2"
			load_flags = 30;
		}

		if (!load_raw && (maximum = 0xfff))
		{
			load_raw = &LibRaw::unpacked_load_raw;
		}
		if (!strncmp(model, "DiMAGE A",
			8)) // Minolta "DiMAGE A1", "DiMAGE A2", "DiMAGE A200"
		{
			if (!strcmp(model, "DiMAGE A200"))
				filters = 0x49494949;
			tiff_bps = 12;
			load_raw = &LibRaw::packed_load_raw;
		}
		else if (!strncmp(normalized_model, "DG-", 3))
		{
			load_raw = &LibRaw::packed_load_raw;
		}
		else if (!strncmp(model, "DiMAGE G",
			8)) // hack Minolta "DiMAGE G400", "DiMAGE G500",
				// "DiMAGE G530", "DiMAGE G600"
		{
			if (model[8] == '4') // DiMAGE G400
			{
				height = 1716;
				width = 2304;
			}
			else if (model[8] == '5') // DiMAGE G500 / G530
			{
			konica_510z:
				height = 1956;
				width = 2607;
				raw_width = 2624;
			}
			else if (model[8] == '6') // DiMAGE G600
			{
				height = 2136;
				width = 2848;
			}
			data_offset += 14;
			filters = 0x61616161;
		konica_400z:
			load_raw = &LibRaw::unpacked_load_raw;
			maximum = 0x3df;
			order = 0x4d4d;
		}

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Samsung)) {
		if (raw_width == 4704) // Samsung NX100, NX10, NX11,
		{
			height -= top_margin = 8;
			width -= 2 * (left_margin = 8);
			load_flags = 32;
		}
		else if (!strcmp(model, "NX3000")) // Samsung NX3000; raw_width: 5600
		{
			top_margin = 38;
			left_margin = 92;
			width = 5456;
			height = 3634;
			filters = 0x61616161;
			colors = 3;
		}
		else if (raw_height == 3714) // Samsung NX2000, NX300M, NX300, NX30, EK-GN120
		{
			height -= top_margin = 18;
			left_margin = raw_width - (width = 5536);
			if (raw_width != 5600)
				left_margin = top_margin = 0;
			filters = 0x61616161;
			colors = 3;
		}
		else if (raw_width == 5632) // Samsung NX1000, NX200, NX20, NX210
		{
			order = 0x4949;
			height = 3694;
			top_margin = 2;
			width = 5574 - (left_margin = 32 + tiff_bps);
			if (tiff_bps == 12)
				load_flags = 80;
		}
		else if (raw_width == 5664) // Samsung "NX mini"
		{
			height -= top_margin = 17;
			left_margin = 96;
			width = 5544;
			filters = 0x49494949;
		}
		else if (raw_width == 6496) // Samsung NX1, NX500
		{
			filters = 0x61616161;
			if (!black && !cblack[0] && !cblack[1] && !cblack[2] && !cblack[3])
				black = 1 << (tiff_bps - 7);
		}
		else if (!strcmp(model, "EX1")) // Samsung EX1; raw_width: 3688
		{
			order = 0x4949;
			height -= 20;
			top_margin = 2;
			if ((width -= 6) > 3682)
			{
				height -= 10;
				width -= 46;
				top_margin = 8;
			}
		}
		else if (!strcmp(model, "WB2000")) // Samsung WB2000; raw_width: 3728
		{
			order = 0x4949;
			height -= 3;
			top_margin = 2;
			if ((width -= 10) > 3718)
			{
				height -= 28;
				width -= 56;
				top_margin = 8;
			}
		}
		else if (!strcmp(model, "WB550")) // Samsung WB550; raw_width: 4000
		{
			order = 0x4949;
		}
		else if (!strcmp(model, "EX2F")) // Samsung EX2F; raw_width: 4176
		{
			height = 3030;
			width = 4040;
			top_margin = 15;
			left_margin = 24;
			order = 0x4949;
			filters = 0x49494949;
			load_raw = &LibRaw::unpacked_load_raw;
		}
	}

	else if (makeIs(LIBRAW_CAMERAMAKER_ST_Micro) && !strcmp(model, "STV680 VGA"))
	{
		black = 16;
	}
	else if (!strcmp(model, "N95"))
	{
		height = raw_height - (top_margin = 2);
	}
	else if (!strcmp(model, "640x480"))
	{
		gamma_curve(0.45, 4.5, 1, 255);
	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Hasselblad))
	{
		if (load_raw == &LibRaw::lossless_jpeg_load_raw)
			load_raw = &LibRaw::hasselblad_load_raw;

		if ((imHassy.SensorCode == 4) && !strncmp(model, "V96C", 4)) { // Hasselblad V96C
			strcpy(model, "V96C");
			strcpy(normalized_model, model);
			height -= (top_margin = 6);
			width -= (left_margin = 3) + 7;
			filters = 0x61616161;

		}
		else if ((imHassy.SensorCode == 9) && imHassy.uncropped) { // various Hasselblad '-39'
			height = 5444;
			width = 7248;
			top_margin = 4;
			left_margin = 7;
			filters = 0x61616161;

		}
		else if ((imHassy.SensorCode == 13) && imHassy.uncropped) { // Hasselblad H4D-40, H5D-40
			height -= 84;
			width -= 82;
			top_margin = 4;
			left_margin = 41;
			filters = 0x61616161;

		}
		else if ((imHassy.SensorCode == 11) && imHassy.uncropped) { // Hasselblad H5D-50
			height -= 84;
			width -= 82;
			top_margin = 4;
			left_margin = 41;
			filters = 0x61616161;

		}
		else if ((imHassy.SensorCode == 15) &&
			!imHassy.SensorSubCode && // Hasselblad H5D-50c
			imHassy.uncropped) {
			left_margin = 52;
			top_margin = 100;
			width = 8272;
			height = 6200;
			black = 256;

		}
		else if ((imHassy.SensorCode == 15) &&
			(imHassy.SensorSubCode == 2) && // various Hasselblad X1D cameras
			imHassy.uncropped) {
			top_margin = 96;
			height -= 96;
			left_margin = 48;
			width -= 106;
			maximum = 0xffff;
			tiff_bps = 16;

		}
		else if ((imHassy.SensorCode == 12) && imHassy.uncropped) { // Hasselblad H4D-60
			if (black > 500) { // (imHassy.format == LIBRAW_HF_FFF)
				top_margin = 12;
				left_margin = 44;
				width = 8956;
				height = 6708;
				memset(cblack, 0, sizeof(cblack));
				black = 512;
			}
			else { // (imHassy.format == LIBRAW_HF_3FR)
				top_margin = 8;
				left_margin = 40;
				width = 8964;
				height = 6716;
				black += load_flags = 256;
				maximum = 0x8101;
			}

		}
		else if ((imHassy.SensorCode == 17) && imHassy.uncropped) { // Hasselblad H6D-100c, A6D-100c
			left_margin = 64;
			width = 11608;
			top_margin = 108;
			height = raw_height - top_margin;
		}

		if (tiff_samples > 1)
		{
			is_raw = tiff_samples + 1;
			if (!shot_select && !half_size)
				filters = 0;
		}
	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Sinar))
	{
		if (!load_raw)
			load_raw = &LibRaw::unpacked_load_raw;
		if (is_raw > 1 && !shot_select)
			filters = 0;
		maximum = 0x3fff;
	}

	if (load_raw == &LibRaw::sinar_4shot_load_raw)
	{
		if (is_raw > 1 && !shot_select)
			filters = 0;
	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Leaf))
	{
		maximum = 0x3fff;
		fseek(ifp, data_offset, SEEK_SET);
		if (ljpeg_start(&jh, 1) && jh.bits == 15)
			maximum = 0x1fff;
		if (tiff_samples > 1)
			filters = 0;
		if (tiff_samples > 1 || tile_length < raw_height)
		{
			load_raw = &LibRaw::leaf_hdr_load_raw;
			raw_width = tile_width;
		}
		if ((width | height) == 2048)
		{
			if (tiff_samples == 1)
			{
				filters = 1;
				strcpy(cdesc, "RBTG");
				strcpy(model, "CatchLight");
				strcpy(normalized_model, model);
				top_margin = 8;
				left_margin = 18;
				height = 2032;
				width = 2016;
			}
			else
			{
				strcpy(model, "DCB2");
				strcpy(normalized_model, model);
				top_margin = 10;
				left_margin = 16;
				height = 2028;
				width = 2022;
			}
		}
		else if (width + height == 3144 + 2060)
		{
			if (!model[0])
			{
				strcpy(model, "Cantare");
				strcpy(normalized_model, model);
			}
			if (width > height)
			{
				top_margin = 6;
				left_margin = 32;
				height = 2048;
				width = 3072;
				filters = 0x61616161;
			}
			else
			{
				left_margin = 6;
				top_margin = 32;
				width = 2048;
				height = 3072;
				filters = 0x16161616;
			}
			if (!cam_mul[0] || model[0] == 'V')
				filters = 0;
			else
				is_raw = tiff_samples;
		}
		else if (width == 2116) // Leaf "Valeo 6"
		{
			strcpy(model, "Valeo 6");
			strcpy(normalized_model, model);
			height -= 2 * (top_margin = 30);
			width -= 2 * (left_margin = 55);
			filters = 0x49494949;
		}
		else if (width == 3171) // Leaf "Valeo 6"
		{
			strcpy(model, "Valeo 6");
			strcpy(normalized_model, model);
			height -= 2 * (top_margin = 24);
			width -= 2 * (left_margin = 24);
			filters = 0x16161616;
		}
	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Panasonic))
	{
		if (raw_width > 0 &&
			((flen - data_offset) / (raw_width * 8 / 7) == raw_height))
			load_raw = &LibRaw::panasonic_load_raw;
		if (!load_raw)
		{
			load_raw = &LibRaw::unpacked_load_raw;
			load_flags = 4;
		}
		zero_is_bad = 1;
		if ((height += 12) > raw_height)
			height = raw_height;
		for (i = 0; i < int(sizeof pana / sizeof *pana); i++)
			if (raw_width == pana[i][0] && raw_height == pana[i][1])
			{
				left_margin = pana[i][2];
				top_margin = pana[i][3];
				width += pana[i][4];
				height += pana[i][5];
			}
		if (!tiff_bps && pana_bpp >= 12 && pana_bpp <= 14)
			tiff_bps = pana_bpp;

		filters = 0x01010101U *
			(uchar) "\x94\x61\x49\x16"[((filters - 1) ^ (left_margin & 1) ^
			(top_margin << 1)) &
			3];

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Contax) &&
		!strcmp(model, "N Digital")) {
		height = 2047;
		width = 3072;
		filters = 0x61616161;
		data_offset = 0x1a00;
		load_raw = &LibRaw::packed_load_raw;

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Sony)) {
		if (!strcmp(model, "DSC-F828")) { // Sony DSC-F828
			width = 3288;
			left_margin = 5;
			mask[1][3] = -17;
			data_offset = 862144;
			load_raw = &LibRaw::sony_load_raw;
			filters = 0x9c9c9c9c;
			colors = 4;
			strcpy(cdesc, "RGBE");

		}
		else if (!strcmp(model, "DSC-V3")) { // Sony DSC-V3
			width = 3109;
			left_margin = 59;
			mask[0][1] = 9;
			data_offset = 787392;
			load_raw = &LibRaw::sony_load_raw;

		}
		else if (raw_width == 3984) { // Sony DSC-R1;
			width = 3925;
			order = 0x4d4d;

		}
		else if (raw_width == 4288) { // Sony ILCE-7S, ILCE-7SM2, DSLR-A700, DSLR-A500;
			width -= 32;

		}
		else if (raw_width == 4600) { // Sony DSLR-A290, DSLR-A350, DSLR-A380;
			if (!strcmp(model, "DSLR-A350"))
				height -= 4;
			black = 0;

		}
		else if (raw_width == 4928) {
			// Sony DSLR-A580, NEX-C3, SLT-A35, DSC-HX99, SLT-A55,
			// NEX-5N, SLT-A37, SLT-A57, NEX-F3, NEX-6, NEX-5R, NEX-3N, NEX-5T;
			if (height < 3280)
				width -= 8;

		}
		else if (raw_width == 5504) {
			// Sony ILCE-3000, SLT-A58, DSC-RX100M3, ILCE-QX1,
			// DSC-RX10M4, DSC-RX100M6, DSC-RX100, DSC-RX100M2, DSC-RX10,
			// ILCE-5000, DSC-RX100M4, DSC-RX10M2, DSC-RX10M3,
			// DSC-RX100M5, DSC-RX100M5A;
			width -= height > 3664 ? 8 : 32;

		}
		else if (raw_width == 6048) {
			// Sony SLT-A65, DSC-RX1, SLT-A77, DSC-RX1, ILCA-77M2,
			// ILCE-7M3, NEX-7, SLT-A99, ILCE-7, DSC-RX1R, ILCE-6000,
			// ILCE-5100, ILCE-7M2, ILCA-68, ILCE-6300, ILCE-9,
			// ILCE-6500, ILCE-6400;
			width -= 24;
			if (strstr(normalized_model, "RX1") ||
				strstr(normalized_model, "A99"))
				width -= 6;

		}
		else if (raw_width == 7392) { // Sony ILCE-7R;
			width -= 30;

		}
		else if (raw_width == 8000) {
			// Sony ILCE-7RM2, ILCE-7RM2, ILCE-7RM3, DSC-RX1RM2, ILCA-99M2;
			width -= 32;

		}
		else if (raw_width == 9600) { // Sony ILCE-7RM4
			width -= 32;

		}
		else if (!strcmp(model, "DSLR-A100")) {
			if (width == 3880) {
				height--;
				width = ++raw_width;
			}
			else {
				height -= 4;
				width -= 4;
				order = 0x4d4d;
				load_flags = 2;
			}
			filters = 0x61616161;
		}
	}

	else if (!strcmp(model, "PIXL")) {
		height -= top_margin = 4;
		width -= left_margin = 32;
		gamma_curve(0, 7, 1, 255);

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Kodak)) {

		if (!strncasecmp(model, "EasyShare", 9)) {
			data_offset = data_offset < 0x15000 ? 0x15000 : 0x17000;
			load_raw = &LibRaw::packed_load_raw;

		}
		else if (!strcmp(model, "C603") ||
			!strcmp(model, "C330") ||
			!strcmp(model, "12MP")) {
			order = 0x4949;
			if (filters && data_offset) {
				fseek(ifp, data_offset < 4096 ? 168 : 5252, SEEK_SET);
				read_shorts(curve, 256);
			}
			else
				gamma_curve(0, 3.875, 1, 255);

			load_raw = filters ? &LibRaw::eight_bit_load_raw
				: strcmp(model, "C330") ? &LibRaw::kodak_c603_load_raw
				: &LibRaw::kodak_c330_load_raw;
			load_flags = tiff_bps > 16;
			tiff_bps = 8;

		}
		else {
			if (!strncmp(model, "NC2000", 6) ||
				!strncmp(model, "EOSDCS", 6) ||
				!strncmp(model, "DCS4", 4)) {
				width -= 4;
				left_margin = 2;

			}
			else if (!strcmp(model, "DCS660M")) {
				black = 214;

			}
			else if (!strcmp(model, "EOS D2000C")) {
				filters = 0x61616161;
				if (!black) black = curve[200];
			}

			if (filters == UINT_MAX) filters = 0x61616161;

			if (!strcmp(model + 4, "20X"))
				strcpy(cdesc, "MYCY");
			if (!strcmp(model, "DC25")) {
				data_offset = 15424;
			}

			if (!strncmp(model, "DC2", 3)) {
				raw_height = 2 + (height = 242);
				if (!strncmp(model, "DC290", 5))
					iso_speed = 100;
				if (!strncmp(model, "DC280", 5))
					iso_speed = 70;
				if (flen < 100000) {
					raw_width = 256;
					width = 249;
					pixel_aspect = (4.0 * height) / (3.0 * width);
				}
				else {
					raw_width = 512;
					width = 501;
					pixel_aspect = (493.0 * height) / (373.0 * width);
				}
				top_margin = left_margin = 1;
				colors = 4;
				filters = 0x8d8d8d8d;
				simple_coeff(1);
				pre_mul[1] = 1.179;
				pre_mul[2] = 1.209;
				pre_mul[3] = 1.036;
				load_raw = &LibRaw::eight_bit_load_raw;

			}
			else if (!strcmp(model, "DC40")) {
				height = 512;
				width = 768;
				data_offset = 1152;
				load_raw = &LibRaw::kodak_radc_load_raw;
				tiff_bps = 12;
				FORC4 cam_mul[c] = 1.0f;

			}
			else if (!strcmp(model, "DC50")) {
				height = 512;
				width = 768;
				iso_speed = 84;
				data_offset = 19712;
				load_raw = &LibRaw::kodak_radc_load_raw;
				FORC4 cam_mul[c] = 1.0f;

			}
			else if (!strcmp(model, "DC120")) {
				raw_height = height = 976;
				raw_width = width = 848;
				iso_speed = 160;
				pixel_aspect = height / 0.75 / width;
				load_raw = tiff_compress == 7 ? &LibRaw::kodak_jpeg_load_raw
					: &LibRaw::kodak_dc120_load_raw;

			}
			else if (!strcmp(model, "DCS200")) {
				thumb_height = 128;
				thumb_width = 192;
				thumb_offset = 6144;
				thumb_misc = 360;
				iso_speed = 140;
				write_thumb = &LibRaw::layer_thumb;
				black = 17;
			}
		}

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Logitech) &&
		!strcmp(model, "Fotoman Pixtura")) {
		height = 512;
		width = 768;
		data_offset = 3632;
		load_raw = &LibRaw::kodak_radc_load_raw;
		filters = 0x61616161;
		simple_coeff(2);

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Apple) &&
		!strncmp(model, "QuickTake", 9)) {
		if (head[5])
			strcpy(model + 10, "200");
		fseek(ifp, 544, SEEK_SET);
		height = get2();
		width = get2();
		data_offset = (get4(), get2()) == 30 ? 738 : 736;
		if (height > width) {
			SWAP(height, width);
			fseek(ifp, data_offset - 6, SEEK_SET);
			flip = ~get2() & 3 ? 5 : 6;
		}
		filters = 0x61616161;

	}
	else if (makeIs(LIBRAW_CAMERAMAKER_Rollei) &&
		!load_raw) {
		switch (raw_width) {
		case 1316: // Rollei d530flex
			height = 1030;
			width = 1300;
			top_margin = 1;
			left_margin = 6;
			break;
		case 2568:
			height = 1960;
			width = 2560;
			top_margin = 2;
			left_margin = 8;
		}
		filters = 0x16161616;
		load_raw = &LibRaw::rollei_load_raw;

	}
	else if (!strcmp(model, "GRAS-50S5C")) {
		height = 2048;
		width = 2440;
		load_raw = &LibRaw::unpacked_load_raw;
		data_offset = 0;
		filters = 0x49494949;
		order = 0x4949;
		maximum = 0xfffC;

	}
	else if (!strcmp(model, "BB-500CL")) {
		height = 2058;
		width = 2448;
		load_raw = &LibRaw::unpacked_load_raw;
		data_offset = 0;
		filters = 0x94949494;
		order = 0x4949;
		maximum = 0x3fff;

	}
	else if (!strcmp(model, "BB-500GE")) {
		height = 2058;
		width = 2456;
		load_raw = &LibRaw::unpacked_load_raw;
		data_offset = 0;
		filters = 0x94949494;
		order = 0x4949;
		maximum = 0x3fff;

	}
	else if (!strcmp(model, "SVS625CL")) {
		height = 2050;
		width = 2448;
		load_raw = &LibRaw::unpacked_load_raw;
		data_offset = 0;
		filters = 0x94949494;
		order = 0x4949;
		maximum = 0x0fff;
	}
}