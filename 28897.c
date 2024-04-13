poppler_print_annot_cb (Annot *annot, void *user_data)
{
  if (annot->getFlags () & Annot::flagPrint)
    return gTrue;
  return (annot->getType() == Annot::typeWidget);
}
