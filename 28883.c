poppler_page_get_transition (PopplerPage *page)
{
  PageTransition *trans;
  PopplerPageTransition *transition;
  Object obj;
  
  g_return_val_if_fail (POPPLER_IS_PAGE (page), NULL);

  trans = new PageTransition (page->page->getTrans (&obj));
  obj.free ();

  if (!trans->isOk ()) {
    delete trans;
    return NULL;
  }

  transition = poppler_page_transition_new ();

  switch (trans->getType ())
    {
      case transitionReplace:
        transition->type = POPPLER_PAGE_TRANSITION_REPLACE;
	break;
      case transitionSplit:
	transition->type = POPPLER_PAGE_TRANSITION_SPLIT;
	break;
      case transitionBlinds:
        transition->type = POPPLER_PAGE_TRANSITION_BLINDS;
	break;
      case transitionBox:
        transition->type = POPPLER_PAGE_TRANSITION_BOX;
	break;
      case transitionWipe:
        transition->type = POPPLER_PAGE_TRANSITION_WIPE;
	break;
      case transitionDissolve:
        transition->type = POPPLER_PAGE_TRANSITION_DISSOLVE;
	break;
      case transitionGlitter:
        transition->type = POPPLER_PAGE_TRANSITION_GLITTER;
	break;
      case transitionFly:
        transition->type = POPPLER_PAGE_TRANSITION_FLY;
	break;
      case transitionPush:
        transition->type = POPPLER_PAGE_TRANSITION_PUSH;
	break;
      case transitionCover:
        transition->type = POPPLER_PAGE_TRANSITION_COVER;
	break;
      case transitionUncover:
        transition->type = POPPLER_PAGE_TRANSITION_UNCOVER;
        break;
      case transitionFade:
        transition->type = POPPLER_PAGE_TRANSITION_FADE;
	break;
      default:
        g_assert_not_reached ();
    }

  transition->alignment = (trans->getAlignment() == transitionHorizontal) ?
	  POPPLER_PAGE_TRANSITION_HORIZONTAL :
	  POPPLER_PAGE_TRANSITION_VERTICAL;

  transition->direction = (trans->getDirection() == transitionInward) ?
	  POPPLER_PAGE_TRANSITION_INWARD :
	  POPPLER_PAGE_TRANSITION_OUTWARD;

  transition->duration = trans->getDuration();
  transition->angle = trans->getAngle();
  transition->scale = trans->getScale();
  transition->rectangular = trans->isRectangular();
  
  delete trans;
  
  return transition;
}
