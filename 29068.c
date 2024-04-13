unsigned int GfxColorSpace::getCMSNChannels(icColorSpaceSignature cs)
{
    switch (cs) {
    case icSigXYZData:
    case icSigLuvData:
    case icSigLabData:
    case icSigYCbCrData:
    case icSigYxyData:
    case icSigRgbData:
    case icSigHsvData:
    case icSigHlsData:
    case icSigCmyData:
    case icSig3colorData:
      return 3;
      break;
    case icSigGrayData:
      return 1;
      break;
    case icSigCmykData:
    case icSig4colorData:
      return 4;
      break;
    case icSig2colorData:
      return 2;
      break;
    case icSig5colorData:
      return 5;
      break;
    case icSig6colorData:
      return 6;
      break;
    case icSig7colorData:
      return 7;
      break;
    case icSig8colorData:
      return 8;
      break;
    case icSig9colorData:
      return 9;
      break;
    case icSig10colorData:
      return 10;
      break;
    case icSig11colorData:
      return 11;
      break;
    case icSig12colorData:
      return 12;
      break;
    case icSig13colorData:
      return 13;
      break;
    case icSig14colorData:
      return 14;
      break;
    case icSig15colorData:
      return 15;
    default:
      break;
    }
    return 3;
}
