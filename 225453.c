static const char *poly_basis_str(ssize_t n)
{
  /* return the result for this polynomial term */
  switch(n) {
    case  0:  return(""); /* constant */
    case  1:  return("*ii");
    case  2:  return("*jj"); /* affine                order = 1   terms = 3 */
    case  3:  return("*ii*jj"); /* bilinear           order = 1.5 terms = 4 */
    case  4:  return("*ii*ii");
    case  5:  return("*jj*jj"); /* quadratic          order = 2   terms = 6 */
    case  6:  return("*ii*ii*ii");
    case  7:  return("*ii*ii*jj");
    case  8:  return("*ii*jj*jj");
    case  9:  return("*jj*jj*jj"); /* cubic           order = 3   terms = 10 */
    case 10:  return("*ii*ii*ii*ii");
    case 11:  return("*ii*ii*ii*jj");
    case 12:  return("*ii*ii*jj*jj");
    case 13:  return("*ii*jj*jj*jj");
    case 14:  return("*jj*jj*jj*jj"); /* quartic      order = 4   terms = 15 */
    case 15:  return("*ii*ii*ii*ii*ii");
    case 16:  return("*ii*ii*ii*ii*jj");
    case 17:  return("*ii*ii*ii*jj*jj");
    case 18:  return("*ii*ii*jj*jj*jj");
    case 19:  return("*ii*jj*jj*jj*jj");
    case 20:  return("*jj*jj*jj*jj*jj"); /* quintic   order = 5   terms = 21 */
  }
  return( "UNKNOWN" ); /* should never happen */
}