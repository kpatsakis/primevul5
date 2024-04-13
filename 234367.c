is_array_assignment()
{
    udvt_entry *udv;
    struct value newvalue;
    int index;
    TBOOLEAN looks_OK = FALSE;
    int brackets;

    if (!isletter(c_token) || !equals(c_token+1, "["))
	return FALSE;

    /* There are other legal commands where the 2nd token is [
     * e.g.  "plot [min:max] foo"
     * so we check that the closing ] is immediately followed by =.
     */
    for (index=c_token+2, brackets=1; index < num_tokens; index++) {
	if (equals(index,";"))
	    return FALSE;
	if (equals(index,"["))
	    brackets++;
	if (equals(index,"]"))
	    brackets--;
	if (brackets == 0) {
	    if (!equals(index+1,"="))
		return FALSE;
	    looks_OK = TRUE;
	    break;
	}
    }
    if (!looks_OK)
	return FALSE;

    udv = add_udv(c_token);
    if (udv->udv_value.type != ARRAY)
	int_error(c_token, "Not a known array");

    /* Evaluate index */
    c_token += 2;
    index = int_expression();
    if (index <= 0 || index > udv->udv_value.v.value_array[0].v.int_val)
	int_error(c_token, "array index out of range");
    if (!equals(c_token, "]") || !equals(c_token+1, "="))
	int_error(c_token, "Expecting Arrayname[<expr>] = <expr>");

    /* Evaluate right side of assignment */
    c_token += 2;
    (void) const_express(&newvalue);
    udv->udv_value.v.value_array[index] = newvalue;

    return TRUE;
}