poppler_page_transition_copy (PopplerPageTransition *transition)
{
  PopplerPageTransition *new_transition;

  new_transition = poppler_page_transition_new ();
  *new_transition = *transition;
  
  return new_transition;
}
