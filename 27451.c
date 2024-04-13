 MagickExport int LocaleLowercase(const int c)
 {
 #if defined(MAGICKCORE_LOCALE_SUPPORT)
   if (c_locale != (locale_t) NULL)
     return(tolower_l((int) ((unsigned char) c),c_locale));
#endif
  return(tolower((int) ((unsigned char) c)));
}
