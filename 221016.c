dnp3_al_empty_obj(guint16 al_obj)
{

  /* return a TRUE if we expect an empty object (default var, class object, etc) */
  switch (al_obj)
  {
    case AL_OBJ_BI_ALL:      /* Binary Input Default Variation (Obj:01, Var:Default) */
    case AL_OBJ_BIC_ALL:     /* Binary Input Change Default Variation (Obj:02, Var:Default) */
    case AL_OBJ_BOC_ALL:     /* Binary Output Event Default Variation (Obj:11, Var:Default) */
    case AL_OBJ_2BI_ALL:     /* Double-bit Input Default Variation (Obj:03, Var:Default) */
    case AL_OBJ_2BIC_ALL:    /* Double-bit Input Change Default Variation (Obj:04, Var:Default) */
    case AL_OBJ_CTR_ALL:     /* Binary Counter Default Variation (Obj:20, Var:Default) */
    case AL_OBJ_CTRC_ALL:    /* Binary Counter Change Default Variation (Obj:22 Var:Default) */
    case AL_OBJ_AI_ALL:      /* Analog Input Default Variation (Obj:30, Var:Default) */
    case AL_OBJ_AIC_ALL:     /* Analog Input Change Default Variation (Obj:32 Var:Default) */
    case AL_OBJ_AIDB_ALL:    /* Analog Input Deadband Default Variation (Obj:34, Var:Default) */
    case AL_OBJ_AOC_ALL:     /* Analog Output Event Default Variation (Obj:42 Var:Default) */
    case AL_OBJ_CLASS0:      /* Class Data Objects */
    case AL_OBJ_CLASS1:
    case AL_OBJ_CLASS2:
    case AL_OBJ_CLASS3:
      return TRUE;
      break;
    default:
      return FALSE;
      break;
  }
}