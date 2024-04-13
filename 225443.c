static size_t poly_number_terms(double order)
{
 /* Return the number of terms for a 2d polynomial */
  if ( order < 1 || order > 5 ||
       ( order != floor(order) && (order-1.5) > MagickEpsilon) )
    return 0; /* invalid polynomial order */
  return((size_t) floor((order+1)*(order+2)/2));
}