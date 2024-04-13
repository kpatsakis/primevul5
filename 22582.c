uniname2ctype_hash (str, len)
     register const char *str;
     register unsigned int len;
{
#ifndef USE_UNICODE_PROPERTIES
  static const unsigned char asso_values[] =
#else /* USE_UNICODE_PROPERTIES */
  static const unsigned short asso_values[] =
#endif /* USE_UNICODE_PROPERTIES */
    {
#ifndef USE_UNICODE_PROPERTIES
      22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
      22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
      22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
      22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
      22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
      22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
      22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
      22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
      22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
      22, 22, 22, 22, 22, 22, 22,  3, 13,  6,
       4, 22, 22, 11, 22,  1, 22, 22, 10, 22,
       2, 22,  1, 22, 10,  8,  4,  7, 22,  3,
       4, 22, 22, 22, 22, 22, 22, 22
#else /* USE_UNICODE_PROPERTIES */
      1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742,
      1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742,
      1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742,
      1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742,
      1742, 1742, 1742, 1742, 1742, 1742,    2, 1742,    9,    1,
         2,   18,    5,    3,    4, 1742, 1742, 1742, 1742, 1742,
      1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742,
      1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742,
      1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742, 1742,
      1742, 1742, 1742, 1742, 1742, 1742, 1742,    8,  280,    6,
        96,   67,  362,  294,   38,    9,   63,  517,    2,  213,
         1,    4,  192,    3,   10,   57,   31,  316,    1,  549,
       330,  567,   36, 1742, 1742, 1742, 1742, 1742
#endif /* USE_UNICODE_PROPERTIES */
    };
#ifndef USE_UNICODE_PROPERTIES
  return len + asso_values[(unsigned char)str[2]] + asso_values[(unsigned char)str[0]];
#else /* USE_UNICODE_PROPERTIES */
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
      case 14:
      case 13:
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
      case 10:
      case 9:
      case 8:
      case 7:
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
#endif /* USE_UNICODE_PROPERTIES */
}