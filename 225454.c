static double poly_basis_fn(ssize_t n, double x, double y)
{
  /* Return the result for this polynomial term */
  switch(n) {
    case  0:  return( 1.0 ); /* constant */
    case  1:  return(  x  );
    case  2:  return(  y  ); /* affine          order = 1   terms = 3 */
    case  3:  return( x*y ); /* bilinear        order = 1.5 terms = 4 */
    case  4:  return( x*x );
    case  5:  return( y*y ); /* quadratic       order = 2   terms = 6 */
    case  6:  return( x*x*x );
    case  7:  return( x*x*y );
    case  8:  return( x*y*y );
    case  9:  return( y*y*y ); /* cubic         order = 3   terms = 10 */
    case 10:  return( x*x*x*x );
    case 11:  return( x*x*x*y );
    case 12:  return( x*x*y*y );
    case 13:  return( x*y*y*y );
    case 14:  return( y*y*y*y ); /* quartic     order = 4   terms = 15 */
    case 15:  return( x*x*x*x*x );
    case 16:  return( x*x*x*x*y );
    case 17:  return( x*x*x*y*y );
    case 18:  return( x*x*y*y*y );
    case 19:  return( x*y*y*y*y );
    case 20:  return( y*y*y*y*y ); /* quintic   order = 5   terms = 21 */
  }
  return( 0 ); /* should never happen */
}