scm_dir_free (SCM p)
{
  if (SCM_DIR_OPEN_P (p))
    closedir ((DIR *) SCM_SMOB_DATA_1 (p));
  return 0;
}