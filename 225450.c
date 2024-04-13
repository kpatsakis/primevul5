static double poly_basis_dy(ssize_t n, double x, double y)
{
  /* polynomial term for y derivative */
  switch(n) {
    case  0:  return( 0.0 ); /* constant */
    case  1:  return( 0.0 );
    case  2:  return( 1.0 ); /* affine      order = 1   terms = 3 */
    case  3:  return(  x  ); /* bilinear    order = 1.5 terms = 4 */
    case  4:  return( 0.0 );
    case  5:  return(  y  ); /* quadratic   order = 2   terms = 6 */
    default:  return( poly_basis_dx(n-1,x,y) ); /* weird but true */
  }
  /* NOTE: the only reason that last is not true for 'quadratic'
     is due to the re-arrangement of terms to allow for 'bilinear'
  */
}