inline void Splash::transform(SplashCoord *matrix,
			      SplashCoord xi, SplashCoord yi,
			      SplashCoord *xo, SplashCoord *yo) {
  *xo = xi * matrix[0] + yi * matrix[2] + matrix[4];
  *yo = xi * matrix[1] + yi * matrix[3] + matrix[5];
}
