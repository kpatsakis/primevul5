static double poly_basis_dx(ssize_t n, double x, double y)
{
  /* polynomial term for x derivative */
  switch(n) {
    case  0:  return( 0.0 ); /* constant */
    case  1:  return( 1.0 );
    case  2:  return( 0.0 ); /* affine      order = 1   terms = 3 */
    case  3:  return(  y  ); /* bilinear    order = 1.5 terms = 4 */
    case  4:  return(  x  );
    case  5:  return( 0.0 ); /* quadratic   order = 2   terms = 6 */
    case  6:  return( x*x );
    case  7:  return( x*y );
    case  8:  return( y*y );
    case  9:  return( 0.0 ); /* cubic       order = 3   terms = 10 */
    case 10:  return( x*x*x );
    case 11:  return( x*x*y );
    case 12:  return( x*y*y );
    case 13:  return( y*y*y );
    case 14:  return( 0.0 ); /* quartic     order = 4   terms = 15 */
    case 15:  return( x*x*x*x );
    case 16:  return( x*x*x*y );
    case 17:  return( x*x*y*y );
    case 18:  return( x*y*y*y );
    case 19:  return( y*y*y*y );
    case 20:  return( 0.0 ); /* quintic     order = 5   terms = 21 */
  }
  return( 0.0 ); /* should never happen */
}