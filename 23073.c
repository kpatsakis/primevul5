cons_macro(const Macro *head, const MacroList *tail) {
	MacroList *list = malloc(sizeof(MacroList));
	list->head = head;
	list->tail = tail;
	return list;
}