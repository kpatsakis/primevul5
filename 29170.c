int GfxColorSpace::setupColorProfiles()
{
  static GBool initialized = gFalse;
  cmsHTRANSFORM transform;
  unsigned int nChannels;

  if (initialized) return 0;
  initialized = gTrue;

  cmsSetErrorHandler(CMSError);

  if (displayProfile == NULL) {
    if (displayProfileName == NULL) {
      displayProfile = loadColorProfile("display.icc");
    } else if (displayProfileName->getLength() > 0) {
      displayProfile = loadColorProfile(displayProfileName->getCString());
    }
  }
  RGBProfile = loadColorProfile("RGB.icc");
  if (RGBProfile == NULL) {
    /* use built in sRGB profile */
    RGBProfile = cmsCreate_sRGBProfile();
  }
  if (displayProfile != NULL) {
    displayPixelType = getCMSColorSpaceType(cmsGetColorSpace(displayProfile));
    nChannels = getCMSNChannels(cmsGetColorSpace(displayProfile));
    cmsHPROFILE XYZProfile = cmsCreateXYZProfile();
    if ((transform = cmsCreateTransform(XYZProfile, TYPE_XYZ_DBL,
	   displayProfile, 
	   COLORSPACE_SH(displayPixelType) |
	     CHANNELS_SH(nChannels) | BYTES_SH(1),
	  INTENT_RELATIVE_COLORIMETRIC,0)) == 0) {
      error(-1, "Can't create Lab transform");
    } else {
      XYZ2DisplayTransform = new GfxColorTransform(transform);
    }
    cmsCloseProfile(XYZProfile);
  }
  return 0;
}
