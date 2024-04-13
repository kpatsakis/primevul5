static void v4l_fill_fmtdesc(struct v4l2_fmtdesc *fmt)
{
	const unsigned sz = sizeof(fmt->description);
	const char *descr = NULL;
	u32 flags = 0;

	/*
	 * We depart from the normal coding style here since the descriptions
	 * should be aligned so it is easy to see which descriptions will be
	 * longer than 31 characters (the max length for a description).
	 * And frankly, this is easier to read anyway.
	 *
	 * Note that gcc will use O(log N) comparisons to find the right case.
	 */
	switch (fmt->pixelformat) {
	/* Max description length mask:	descr = "0123456789012345678901234567890" */
	case V4L2_PIX_FMT_RGB332:	descr = "8-bit RGB 3-3-2"; break;
	case V4L2_PIX_FMT_RGB444:	descr = "16-bit A/XRGB 4-4-4-4"; break;
	case V4L2_PIX_FMT_ARGB444:	descr = "16-bit ARGB 4-4-4-4"; break;
	case V4L2_PIX_FMT_XRGB444:	descr = "16-bit XRGB 4-4-4-4"; break;
	case V4L2_PIX_FMT_RGBA444:	descr = "16-bit RGBA 4-4-4-4"; break;
	case V4L2_PIX_FMT_RGBX444:	descr = "16-bit RGBX 4-4-4-4"; break;
	case V4L2_PIX_FMT_ABGR444:	descr = "16-bit ABGR 4-4-4-4"; break;
	case V4L2_PIX_FMT_XBGR444:	descr = "16-bit XBGR 4-4-4-4"; break;
	case V4L2_PIX_FMT_BGRA444:	descr = "16-bit BGRA 4-4-4-4"; break;
	case V4L2_PIX_FMT_BGRX444:	descr = "16-bit BGRX 4-4-4-4"; break;
	case V4L2_PIX_FMT_RGB555:	descr = "16-bit A/XRGB 1-5-5-5"; break;
	case V4L2_PIX_FMT_ARGB555:	descr = "16-bit ARGB 1-5-5-5"; break;
	case V4L2_PIX_FMT_XRGB555:	descr = "16-bit XRGB 1-5-5-5"; break;
	case V4L2_PIX_FMT_ABGR555:	descr = "16-bit ABGR 1-5-5-5"; break;
	case V4L2_PIX_FMT_XBGR555:	descr = "16-bit XBGR 1-5-5-5"; break;
	case V4L2_PIX_FMT_RGBA555:	descr = "16-bit RGBA 5-5-5-1"; break;
	case V4L2_PIX_FMT_RGBX555:	descr = "16-bit RGBX 5-5-5-1"; break;
	case V4L2_PIX_FMT_BGRA555:	descr = "16-bit BGRA 5-5-5-1"; break;
	case V4L2_PIX_FMT_BGRX555:	descr = "16-bit BGRX 5-5-5-1"; break;
	case V4L2_PIX_FMT_RGB565:	descr = "16-bit RGB 5-6-5"; break;
	case V4L2_PIX_FMT_RGB555X:	descr = "16-bit A/XRGB 1-5-5-5 BE"; break;
	case V4L2_PIX_FMT_ARGB555X:	descr = "16-bit ARGB 1-5-5-5 BE"; break;
	case V4L2_PIX_FMT_XRGB555X:	descr = "16-bit XRGB 1-5-5-5 BE"; break;
	case V4L2_PIX_FMT_RGB565X:	descr = "16-bit RGB 5-6-5 BE"; break;
	case V4L2_PIX_FMT_BGR666:	descr = "18-bit BGRX 6-6-6-14"; break;
	case V4L2_PIX_FMT_BGR24:	descr = "24-bit BGR 8-8-8"; break;
	case V4L2_PIX_FMT_RGB24:	descr = "24-bit RGB 8-8-8"; break;
	case V4L2_PIX_FMT_BGR32:	descr = "32-bit BGRA/X 8-8-8-8"; break;
	case V4L2_PIX_FMT_ABGR32:	descr = "32-bit BGRA 8-8-8-8"; break;
	case V4L2_PIX_FMT_XBGR32:	descr = "32-bit BGRX 8-8-8-8"; break;
	case V4L2_PIX_FMT_RGB32:	descr = "32-bit A/XRGB 8-8-8-8"; break;
	case V4L2_PIX_FMT_ARGB32:	descr = "32-bit ARGB 8-8-8-8"; break;
	case V4L2_PIX_FMT_XRGB32:	descr = "32-bit XRGB 8-8-8-8"; break;
	case V4L2_PIX_FMT_BGRA32:	descr = "32-bit ABGR 8-8-8-8"; break;
	case V4L2_PIX_FMT_BGRX32:	descr = "32-bit XBGR 8-8-8-8"; break;
	case V4L2_PIX_FMT_RGBA32:	descr = "32-bit RGBA 8-8-8-8"; break;
	case V4L2_PIX_FMT_RGBX32:	descr = "32-bit RGBX 8-8-8-8"; break;
	case V4L2_PIX_FMT_GREY:		descr = "8-bit Greyscale"; break;
	case V4L2_PIX_FMT_Y4:		descr = "4-bit Greyscale"; break;
	case V4L2_PIX_FMT_Y6:		descr = "6-bit Greyscale"; break;
	case V4L2_PIX_FMT_Y10:		descr = "10-bit Greyscale"; break;
	case V4L2_PIX_FMT_Y12:		descr = "12-bit Greyscale"; break;
	case V4L2_PIX_FMT_Y14:		descr = "14-bit Greyscale"; break;
	case V4L2_PIX_FMT_Y16:		descr = "16-bit Greyscale"; break;
	case V4L2_PIX_FMT_Y16_BE:	descr = "16-bit Greyscale BE"; break;
	case V4L2_PIX_FMT_Y10BPACK:	descr = "10-bit Greyscale (Packed)"; break;
	case V4L2_PIX_FMT_Y10P:		descr = "10-bit Greyscale (MIPI Packed)"; break;
	case V4L2_PIX_FMT_Y8I:		descr = "Interleaved 8-bit Greyscale"; break;
	case V4L2_PIX_FMT_Y12I:		descr = "Interleaved 12-bit Greyscale"; break;
	case V4L2_PIX_FMT_Z16:		descr = "16-bit Depth"; break;
	case V4L2_PIX_FMT_INZI:		descr = "Planar 10:16 Greyscale Depth"; break;
	case V4L2_PIX_FMT_CNF4:		descr = "4-bit Depth Confidence (Packed)"; break;
	case V4L2_PIX_FMT_PAL8:		descr = "8-bit Palette"; break;
	case V4L2_PIX_FMT_UV8:		descr = "8-bit Chrominance UV 4-4"; break;
	case V4L2_PIX_FMT_YVU410:	descr = "Planar YVU 4:1:0"; break;
	case V4L2_PIX_FMT_YVU420:	descr = "Planar YVU 4:2:0"; break;
	case V4L2_PIX_FMT_YUYV:		descr = "YUYV 4:2:2"; break;
	case V4L2_PIX_FMT_YYUV:		descr = "YYUV 4:2:2"; break;
	case V4L2_PIX_FMT_YVYU:		descr = "YVYU 4:2:2"; break;
	case V4L2_PIX_FMT_UYVY:		descr = "UYVY 4:2:2"; break;
	case V4L2_PIX_FMT_VYUY:		descr = "VYUY 4:2:2"; break;
	case V4L2_PIX_FMT_YUV422P:	descr = "Planar YUV 4:2:2"; break;
	case V4L2_PIX_FMT_YUV411P:	descr = "Planar YUV 4:1:1"; break;
	case V4L2_PIX_FMT_Y41P:		descr = "YUV 4:1:1 (Packed)"; break;
	case V4L2_PIX_FMT_YUV444:	descr = "16-bit A/XYUV 4-4-4-4"; break;
	case V4L2_PIX_FMT_YUV555:	descr = "16-bit A/XYUV 1-5-5-5"; break;
	case V4L2_PIX_FMT_YUV565:	descr = "16-bit YUV 5-6-5"; break;
	case V4L2_PIX_FMT_YUV32:	descr = "32-bit A/XYUV 8-8-8-8"; break;
	case V4L2_PIX_FMT_AYUV32:	descr = "32-bit AYUV 8-8-8-8"; break;
	case V4L2_PIX_FMT_XYUV32:	descr = "32-bit XYUV 8-8-8-8"; break;
	case V4L2_PIX_FMT_VUYA32:	descr = "32-bit VUYA 8-8-8-8"; break;
	case V4L2_PIX_FMT_VUYX32:	descr = "32-bit VUYX 8-8-8-8"; break;
	case V4L2_PIX_FMT_YUV410:	descr = "Planar YUV 4:1:0"; break;
	case V4L2_PIX_FMT_YUV420:	descr = "Planar YUV 4:2:0"; break;
	case V4L2_PIX_FMT_HI240:	descr = "8-bit Dithered RGB (BTTV)"; break;
	case V4L2_PIX_FMT_HM12:		descr = "YUV 4:2:0 (16x16 Macroblocks)"; break;
	case V4L2_PIX_FMT_M420:		descr = "YUV 4:2:0 (M420)"; break;
	case V4L2_PIX_FMT_NV12:		descr = "Y/CbCr 4:2:0"; break;
	case V4L2_PIX_FMT_NV21:		descr = "Y/CrCb 4:2:0"; break;
	case V4L2_PIX_FMT_NV16:		descr = "Y/CbCr 4:2:2"; break;
	case V4L2_PIX_FMT_NV61:		descr = "Y/CrCb 4:2:2"; break;
	case V4L2_PIX_FMT_NV24:		descr = "Y/CbCr 4:4:4"; break;
	case V4L2_PIX_FMT_NV42:		descr = "Y/CrCb 4:4:4"; break;
	case V4L2_PIX_FMT_NV12M:	descr = "Y/CbCr 4:2:0 (N-C)"; break;
	case V4L2_PIX_FMT_NV21M:	descr = "Y/CrCb 4:2:0 (N-C)"; break;
	case V4L2_PIX_FMT_NV16M:	descr = "Y/CbCr 4:2:2 (N-C)"; break;
	case V4L2_PIX_FMT_NV61M:	descr = "Y/CrCb 4:2:2 (N-C)"; break;
	case V4L2_PIX_FMT_NV12MT:	descr = "Y/CbCr 4:2:0 (64x32 MB, N-C)"; break;
	case V4L2_PIX_FMT_NV12MT_16X16:	descr = "Y/CbCr 4:2:0 (16x16 MB, N-C)"; break;
	case V4L2_PIX_FMT_YUV420M:	descr = "Planar YUV 4:2:0 (N-C)"; break;
	case V4L2_PIX_FMT_YVU420M:	descr = "Planar YVU 4:2:0 (N-C)"; break;
	case V4L2_PIX_FMT_YUV422M:	descr = "Planar YUV 4:2:2 (N-C)"; break;
	case V4L2_PIX_FMT_YVU422M:	descr = "Planar YVU 4:2:2 (N-C)"; break;
	case V4L2_PIX_FMT_YUV444M:	descr = "Planar YUV 4:4:4 (N-C)"; break;
	case V4L2_PIX_FMT_YVU444M:	descr = "Planar YVU 4:4:4 (N-C)"; break;
	case V4L2_PIX_FMT_SBGGR8:	descr = "8-bit Bayer BGBG/GRGR"; break;
	case V4L2_PIX_FMT_SGBRG8:	descr = "8-bit Bayer GBGB/RGRG"; break;
	case V4L2_PIX_FMT_SGRBG8:	descr = "8-bit Bayer GRGR/BGBG"; break;
	case V4L2_PIX_FMT_SRGGB8:	descr = "8-bit Bayer RGRG/GBGB"; break;
	case V4L2_PIX_FMT_SBGGR10:	descr = "10-bit Bayer BGBG/GRGR"; break;
	case V4L2_PIX_FMT_SGBRG10:	descr = "10-bit Bayer GBGB/RGRG"; break;
	case V4L2_PIX_FMT_SGRBG10:	descr = "10-bit Bayer GRGR/BGBG"; break;
	case V4L2_PIX_FMT_SRGGB10:	descr = "10-bit Bayer RGRG/GBGB"; break;
	case V4L2_PIX_FMT_SBGGR10P:	descr = "10-bit Bayer BGBG/GRGR Packed"; break;
	case V4L2_PIX_FMT_SGBRG10P:	descr = "10-bit Bayer GBGB/RGRG Packed"; break;
	case V4L2_PIX_FMT_SGRBG10P:	descr = "10-bit Bayer GRGR/BGBG Packed"; break;
	case V4L2_PIX_FMT_SRGGB10P:	descr = "10-bit Bayer RGRG/GBGB Packed"; break;
	case V4L2_PIX_FMT_IPU3_SBGGR10: descr = "10-bit bayer BGGR IPU3 Packed"; break;
	case V4L2_PIX_FMT_IPU3_SGBRG10: descr = "10-bit bayer GBRG IPU3 Packed"; break;
	case V4L2_PIX_FMT_IPU3_SGRBG10: descr = "10-bit bayer GRBG IPU3 Packed"; break;
	case V4L2_PIX_FMT_IPU3_SRGGB10: descr = "10-bit bayer RGGB IPU3 Packed"; break;
	case V4L2_PIX_FMT_SBGGR10ALAW8:	descr = "8-bit Bayer BGBG/GRGR (A-law)"; break;
	case V4L2_PIX_FMT_SGBRG10ALAW8:	descr = "8-bit Bayer GBGB/RGRG (A-law)"; break;
	case V4L2_PIX_FMT_SGRBG10ALAW8:	descr = "8-bit Bayer GRGR/BGBG (A-law)"; break;
	case V4L2_PIX_FMT_SRGGB10ALAW8:	descr = "8-bit Bayer RGRG/GBGB (A-law)"; break;
	case V4L2_PIX_FMT_SBGGR10DPCM8:	descr = "8-bit Bayer BGBG/GRGR (DPCM)"; break;
	case V4L2_PIX_FMT_SGBRG10DPCM8:	descr = "8-bit Bayer GBGB/RGRG (DPCM)"; break;
	case V4L2_PIX_FMT_SGRBG10DPCM8:	descr = "8-bit Bayer GRGR/BGBG (DPCM)"; break;
	case V4L2_PIX_FMT_SRGGB10DPCM8:	descr = "8-bit Bayer RGRG/GBGB (DPCM)"; break;
	case V4L2_PIX_FMT_SBGGR12:	descr = "12-bit Bayer BGBG/GRGR"; break;
	case V4L2_PIX_FMT_SGBRG12:	descr = "12-bit Bayer GBGB/RGRG"; break;
	case V4L2_PIX_FMT_SGRBG12:	descr = "12-bit Bayer GRGR/BGBG"; break;
	case V4L2_PIX_FMT_SRGGB12:	descr = "12-bit Bayer RGRG/GBGB"; break;
	case V4L2_PIX_FMT_SBGGR12P:	descr = "12-bit Bayer BGBG/GRGR Packed"; break;
	case V4L2_PIX_FMT_SGBRG12P:	descr = "12-bit Bayer GBGB/RGRG Packed"; break;
	case V4L2_PIX_FMT_SGRBG12P:	descr = "12-bit Bayer GRGR/BGBG Packed"; break;
	case V4L2_PIX_FMT_SRGGB12P:	descr = "12-bit Bayer RGRG/GBGB Packed"; break;
	case V4L2_PIX_FMT_SBGGR14:	descr = "14-bit Bayer BGBG/GRGR"; break;
	case V4L2_PIX_FMT_SGBRG14:	descr = "14-bit Bayer GBGB/RGRG"; break;
	case V4L2_PIX_FMT_SGRBG14:	descr = "14-bit Bayer GRGR/BGBG"; break;
	case V4L2_PIX_FMT_SRGGB14:	descr = "14-bit Bayer RGRG/GBGB"; break;
	case V4L2_PIX_FMT_SBGGR14P:	descr = "14-bit Bayer BGBG/GRGR Packed"; break;
	case V4L2_PIX_FMT_SGBRG14P:	descr = "14-bit Bayer GBGB/RGRG Packed"; break;
	case V4L2_PIX_FMT_SGRBG14P:	descr = "14-bit Bayer GRGR/BGBG Packed"; break;
	case V4L2_PIX_FMT_SRGGB14P:	descr = "14-bit Bayer RGRG/GBGB Packed"; break;
	case V4L2_PIX_FMT_SBGGR16:	descr = "16-bit Bayer BGBG/GRGR"; break;
	case V4L2_PIX_FMT_SGBRG16:	descr = "16-bit Bayer GBGB/RGRG"; break;
	case V4L2_PIX_FMT_SGRBG16:	descr = "16-bit Bayer GRGR/BGBG"; break;
	case V4L2_PIX_FMT_SRGGB16:	descr = "16-bit Bayer RGRG/GBGB"; break;
	case V4L2_PIX_FMT_SN9C20X_I420:	descr = "GSPCA SN9C20X I420"; break;
	case V4L2_PIX_FMT_SPCA501:	descr = "GSPCA SPCA501"; break;
	case V4L2_PIX_FMT_SPCA505:	descr = "GSPCA SPCA505"; break;
	case V4L2_PIX_FMT_SPCA508:	descr = "GSPCA SPCA508"; break;
	case V4L2_PIX_FMT_STV0680:	descr = "GSPCA STV0680"; break;
	case V4L2_PIX_FMT_TM6000:	descr = "A/V + VBI Mux Packet"; break;
	case V4L2_PIX_FMT_CIT_YYVYUY:	descr = "GSPCA CIT YYVYUY"; break;
	case V4L2_PIX_FMT_KONICA420:	descr = "GSPCA KONICA420"; break;
	case V4L2_PIX_FMT_HSV24:	descr = "24-bit HSV 8-8-8"; break;
	case V4L2_PIX_FMT_HSV32:	descr = "32-bit XHSV 8-8-8-8"; break;
	case V4L2_SDR_FMT_CU8:		descr = "Complex U8"; break;
	case V4L2_SDR_FMT_CU16LE:	descr = "Complex U16LE"; break;
	case V4L2_SDR_FMT_CS8:		descr = "Complex S8"; break;
	case V4L2_SDR_FMT_CS14LE:	descr = "Complex S14LE"; break;
	case V4L2_SDR_FMT_RU12LE:	descr = "Real U12LE"; break;
	case V4L2_SDR_FMT_PCU16BE:	descr = "Planar Complex U16BE"; break;
	case V4L2_SDR_FMT_PCU18BE:	descr = "Planar Complex U18BE"; break;
	case V4L2_SDR_FMT_PCU20BE:	descr = "Planar Complex U20BE"; break;
	case V4L2_TCH_FMT_DELTA_TD16:	descr = "16-bit Signed Deltas"; break;
	case V4L2_TCH_FMT_DELTA_TD08:	descr = "8-bit Signed Deltas"; break;
	case V4L2_TCH_FMT_TU16:		descr = "16-bit Unsigned Touch Data"; break;
	case V4L2_TCH_FMT_TU08:		descr = "8-bit Unsigned Touch Data"; break;
	case V4L2_META_FMT_VSP1_HGO:	descr = "R-Car VSP1 1-D Histogram"; break;
	case V4L2_META_FMT_VSP1_HGT:	descr = "R-Car VSP1 2-D Histogram"; break;
	case V4L2_META_FMT_UVC:		descr = "UVC Payload Header Metadata"; break;
	case V4L2_META_FMT_D4XX:	descr = "Intel D4xx UVC Metadata"; break;
	case V4L2_META_FMT_VIVID:       descr = "Vivid Metadata"; break;
	case V4L2_META_FMT_RK_ISP1_PARAMS:	descr = "Rockchip ISP1 3A Parameters"; break;
	case V4L2_META_FMT_RK_ISP1_STAT_3A:	descr = "Rockchip ISP1 3A Statistics"; break;

	default:
		/* Compressed formats */
		flags = V4L2_FMT_FLAG_COMPRESSED;
		switch (fmt->pixelformat) {
		/* Max description length mask:	descr = "0123456789012345678901234567890" */
		case V4L2_PIX_FMT_MJPEG:	descr = "Motion-JPEG"; break;
		case V4L2_PIX_FMT_JPEG:		descr = "JFIF JPEG"; break;
		case V4L2_PIX_FMT_DV:		descr = "1394"; break;
		case V4L2_PIX_FMT_MPEG:		descr = "MPEG-1/2/4"; break;
		case V4L2_PIX_FMT_H264:		descr = "H.264"; break;
		case V4L2_PIX_FMT_H264_NO_SC:	descr = "H.264 (No Start Codes)"; break;
		case V4L2_PIX_FMT_H264_MVC:	descr = "H.264 MVC"; break;
		case V4L2_PIX_FMT_H264_SLICE:	descr = "H.264 Parsed Slice Data"; break;
		case V4L2_PIX_FMT_H263:		descr = "H.263"; break;
		case V4L2_PIX_FMT_MPEG1:	descr = "MPEG-1 ES"; break;
		case V4L2_PIX_FMT_MPEG2:	descr = "MPEG-2 ES"; break;
		case V4L2_PIX_FMT_MPEG2_SLICE:	descr = "MPEG-2 Parsed Slice Data"; break;
		case V4L2_PIX_FMT_MPEG4:	descr = "MPEG-4 Part 2 ES"; break;
		case V4L2_PIX_FMT_XVID:		descr = "Xvid"; break;
		case V4L2_PIX_FMT_VC1_ANNEX_G:	descr = "VC-1 (SMPTE 412M Annex G)"; break;
		case V4L2_PIX_FMT_VC1_ANNEX_L:	descr = "VC-1 (SMPTE 412M Annex L)"; break;
		case V4L2_PIX_FMT_VP8:		descr = "VP8"; break;
		case V4L2_PIX_FMT_VP8_FRAME:    descr = "VP8 Frame"; break;
		case V4L2_PIX_FMT_VP9:		descr = "VP9"; break;
		case V4L2_PIX_FMT_HEVC:		descr = "HEVC"; break; /* aka H.265 */
		case V4L2_PIX_FMT_HEVC_SLICE:	descr = "HEVC Parsed Slice Data"; break;
		case V4L2_PIX_FMT_FWHT:		descr = "FWHT"; break; /* used in vicodec */
		case V4L2_PIX_FMT_FWHT_STATELESS:	descr = "FWHT Stateless"; break; /* used in vicodec */
		case V4L2_PIX_FMT_CPIA1:	descr = "GSPCA CPiA YUV"; break;
		case V4L2_PIX_FMT_WNVA:		descr = "WNVA"; break;
		case V4L2_PIX_FMT_SN9C10X:	descr = "GSPCA SN9C10X"; break;
		case V4L2_PIX_FMT_PWC1:		descr = "Raw Philips Webcam Type (Old)"; break;
		case V4L2_PIX_FMT_PWC2:		descr = "Raw Philips Webcam Type (New)"; break;
		case V4L2_PIX_FMT_ET61X251:	descr = "GSPCA ET61X251"; break;
		case V4L2_PIX_FMT_SPCA561:	descr = "GSPCA SPCA561"; break;
		case V4L2_PIX_FMT_PAC207:	descr = "GSPCA PAC207"; break;
		case V4L2_PIX_FMT_MR97310A:	descr = "GSPCA MR97310A"; break;
		case V4L2_PIX_FMT_JL2005BCD:	descr = "GSPCA JL2005BCD"; break;
		case V4L2_PIX_FMT_SN9C2028:	descr = "GSPCA SN9C2028"; break;
		case V4L2_PIX_FMT_SQ905C:	descr = "GSPCA SQ905C"; break;
		case V4L2_PIX_FMT_PJPG:		descr = "GSPCA PJPG"; break;
		case V4L2_PIX_FMT_OV511:	descr = "GSPCA OV511"; break;
		case V4L2_PIX_FMT_OV518:	descr = "GSPCA OV518"; break;
		case V4L2_PIX_FMT_JPGL:		descr = "JPEG Lite"; break;
		case V4L2_PIX_FMT_SE401:	descr = "GSPCA SE401"; break;
		case V4L2_PIX_FMT_S5C_UYVY_JPG:	descr = "S5C73MX interleaved UYVY/JPEG"; break;
		case V4L2_PIX_FMT_MT21C:	descr = "Mediatek Compressed Format"; break;
		case V4L2_PIX_FMT_SUNXI_TILED_NV12: descr = "Sunxi Tiled NV12 Format"; break;
		default:
			if (fmt->description[0])
				return;
			WARN(1, "Unknown pixelformat 0x%08x\n", fmt->pixelformat);
			flags = 0;
			snprintf(fmt->description, sz, "%c%c%c%c%s",
					(char)(fmt->pixelformat & 0x7f),
					(char)((fmt->pixelformat >> 8) & 0x7f),
					(char)((fmt->pixelformat >> 16) & 0x7f),
					(char)((fmt->pixelformat >> 24) & 0x7f),
					(fmt->pixelformat & (1UL << 31)) ? "-BE" : "");
			break;
		}
	}

	if (descr)
		WARN_ON(strscpy(fmt->description, descr, sz) < 0);
	fmt->flags |= flags;
}