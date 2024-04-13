char *FoFiType1C::getName() {
  return name ? name->getCString() : (char *)NULL;
}
