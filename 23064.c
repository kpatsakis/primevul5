free_macro_list(const MacroList *list) {
	if (list) {
		free_macro((Macro *)list->head);
		free_macro_list((MacroList *)list->tail);
		free((MacroList *)list);
	}
}