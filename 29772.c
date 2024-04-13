void FoFiType1C::getFontMatrix(double *mat) {
  int i;

  if (topDict.firstOp == 0x0c1e && privateDicts[0].hasFontMatrix) {
    if (topDict.hasFontMatrix) {
      mat[0] = topDict.fontMatrix[0] * privateDicts[0].fontMatrix[0] +
	       topDict.fontMatrix[1] * privateDicts[0].fontMatrix[2];
      mat[1] = topDict.fontMatrix[0] * privateDicts[0].fontMatrix[1] +
               topDict.fontMatrix[1] * privateDicts[0].fontMatrix[3];
      mat[2] = topDict.fontMatrix[2] * privateDicts[0].fontMatrix[0] +
	       topDict.fontMatrix[3] * privateDicts[0].fontMatrix[2];
      mat[3] = topDict.fontMatrix[2] * privateDicts[0].fontMatrix[1] +
	       topDict.fontMatrix[3] * privateDicts[0].fontMatrix[3];
      mat[4] = topDict.fontMatrix[4] * privateDicts[0].fontMatrix[0] +
	       topDict.fontMatrix[5] * privateDicts[0].fontMatrix[2];
      mat[5] = topDict.fontMatrix[4] * privateDicts[0].fontMatrix[1] +
	       topDict.fontMatrix[5] * privateDicts[0].fontMatrix[3];
    } else {
      for (i = 0; i < 6; ++i) {
	mat[i] = privateDicts[0].fontMatrix[i];
      }
    }
  } else {
    for (i = 0; i < 6; ++i) {
      mat[i] = topDict.fontMatrix[i];
    }
  }
}
