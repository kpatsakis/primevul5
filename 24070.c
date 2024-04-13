scm_init_filesys ()
{
#ifdef HAVE_POSIX
  scm_tc16_dir = scm_make_smob_type ("directory", 0);
  scm_set_smob_free (scm_tc16_dir, scm_dir_free);
  scm_set_smob_print (scm_tc16_dir, scm_dir_print);

#ifdef O_RDONLY
  scm_c_define ("O_RDONLY", scm_from_int (O_RDONLY));
#endif 	       
#ifdef O_WRONLY
  scm_c_define ("O_WRONLY", scm_from_int (O_WRONLY));
#endif 	       
#ifdef O_RDWR
  scm_c_define ("O_RDWR", scm_from_int (O_RDWR));
#endif 	       
#ifdef O_CREAT
  scm_c_define ("O_CREAT", scm_from_int (O_CREAT));
#endif 	       
#ifdef O_EXCL  
  scm_c_define ("O_EXCL", scm_from_int (O_EXCL));
#endif 	       
#ifdef O_NOCTTY
  scm_c_define ("O_NOCTTY", scm_from_int (O_NOCTTY));
#endif 	       
#ifdef O_TRUNC 
  scm_c_define ("O_TRUNC", scm_from_int (O_TRUNC));
#endif 	       
#ifdef O_APPEND
  scm_c_define ("O_APPEND", scm_from_int (O_APPEND));
#endif 	       
#ifdef O_NONBLOCK
  scm_c_define ("O_NONBLOCK", scm_from_int (O_NONBLOCK));
#endif 	       
#ifdef O_NDELAY
  scm_c_define ("O_NDELAY", scm_from_int (O_NDELAY));
#endif 	       
#ifdef O_SYNC  
  scm_c_define ("O_SYNC", scm_from_int (O_SYNC));
#endif 
#ifdef O_LARGEFILE  
  scm_c_define ("O_LARGEFILE", scm_from_int (O_LARGEFILE));
#endif
#ifdef O_NOTRANS
  scm_c_define ("O_NOTRANS", scm_from_int (O_NOTRANS));
#endif

#ifdef F_DUPFD  
  scm_c_define ("F_DUPFD", scm_from_int (F_DUPFD));
#endif 
#ifdef F_GETFD  
  scm_c_define ("F_GETFD", scm_from_int (F_GETFD));
#endif 
#ifdef F_SETFD  
  scm_c_define ("F_SETFD", scm_from_int (F_SETFD));
#endif 
#ifdef F_GETFL  
  scm_c_define ("F_GETFL", scm_from_int (F_GETFL));
#endif 
#ifdef F_SETFL  
  scm_c_define ("F_SETFL", scm_from_int (F_SETFL));
#endif 
#ifdef F_GETOWN  
  scm_c_define ("F_GETOWN", scm_from_int (F_GETOWN));
#endif 
#ifdef F_SETOWN  
  scm_c_define ("F_SETOWN", scm_from_int (F_SETOWN));
#endif 
#ifdef FD_CLOEXEC  
  scm_c_define ("FD_CLOEXEC", scm_from_int (FD_CLOEXEC));
#endif
#endif /* HAVE_POSIX */

  /* `access' symbols.  */
  scm_c_define ("R_OK", scm_from_int (R_OK));
  scm_c_define ("W_OK", scm_from_int (W_OK));
  scm_c_define ("X_OK", scm_from_int (X_OK));
  scm_c_define ("F_OK", scm_from_int (F_OK));

  scm_dot_string = scm_from_locale_string (".");

#include "libguile/filesys.x"
}