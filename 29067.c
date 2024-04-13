unsigned int GfxColorSpace::getCMSColorSpaceType(icColorSpaceSignature cs)
{
    switch (cs) {
    case icSigXYZData:
      return PT_XYZ;
      break;
    case icSigLabData:
      return PT_Lab;
      break;
    case icSigLuvData:
      return PT_YUV;
      break;
    case icSigYCbCrData:
      return PT_YCbCr;
      break;
    case icSigYxyData:
      return PT_Yxy;
      break;
    case icSigRgbData:
      return PT_RGB;
      break;
    case icSigGrayData:
      return PT_GRAY;
      break;
    case icSigHsvData:
      return PT_HSV;
      break;
    case icSigHlsData:
      return PT_HLS;
      break;
    case icSigCmykData:
      return PT_CMYK;
      break;
    case icSigCmyData:
      return PT_CMY;
      break;
    case icSig2colorData:
    case icSig3colorData:
    case icSig4colorData:
    case icSig5colorData:
    case icSig6colorData:
    case icSig7colorData:
    case icSig8colorData:
    case icSig9colorData:
    case icSig10colorData:
    case icSig11colorData:
    case icSig12colorData:
    case icSig13colorData:
    case icSig14colorData:
    case icSig15colorData:
    default:
      break;
    }
    return PT_RGB;
}
