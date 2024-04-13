*kstring(char *string)
        {
        static char	*null = "[NULL]";

	return ((string == NULL)? null: string);
        }