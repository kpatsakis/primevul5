array_command()
{
    int nsize = 0;	/* Size of array when we leave */
    int est_size = 0;	/* Estimated size */
    struct udvt_entry *array;
    struct value *A;
    int i;

    /* Create or recycle a udv containing an array with the requested name */
    if (!isletter(++c_token))
	int_error(c_token, "illegal variable name");
    array = add_udv(c_token);
    free_value(&array->udv_value);
    c_token++;

    if (equals(c_token, "[")) {
	c_token++;
	nsize = int_expression();
	if (!equals(c_token++,"]"))
	    int_error(c_token-1, "expecting array[size>0]");
    } else if (equals(c_token, "=") && equals(c_token+1, "[")) {
	/* Estimate size of array by counting commas in the initializer */
	for ( i = c_token+2; i < num_tokens; i++) {
	    if (equals(i,",") || equals(i,"]"))
		est_size++;
	    if (equals(i,"]"))
		break;
	}
	nsize = est_size;
    }
    if (nsize <= 0)
	int_error(c_token-1, "expecting array[size>0]");

    array->udv_value.v.value_array = gp_alloc((nsize+1) * sizeof(t_value), "array_command");
    array->udv_value.type = ARRAY;

    /* Element zero of the new array is not visible but contains the size */
    A = array->udv_value.v.value_array;
    A[0].v.int_val = nsize;
    for (i = 0; i <= nsize; i++) {
	A[i].type = NOTDEFINED;
    }

    /* Element zero can also hold an indicator that this is a colormap */
    /* FIXME: more sanity checks?  e.g. all entries INTGR */
    if (equals(c_token, "colormap")) {
	c_token++;
	if (nsize >= 2)	/* Need at least 2 entries to calculate range */
	    A[0].type = COLORMAP_ARRAY;
    }

    /* Initializer syntax:   array A[10] = [x,y,z,,"foo",] */
    if (equals(c_token, "=")) {
	int initializers = 0;
	if (!equals(++c_token, "["))
	    int_error(c_token, "expecting Array[size] = [x,y,...]");
	c_token++;
	for (i = 1; i <= nsize; i++) {
	    if (equals(c_token, "]"))
		break;
	    if (equals(c_token, ",")) {
		initializers++;
		c_token++;
		continue;
	    }
	    const_express(&A[i]);
	    initializers++;
	    if (equals(c_token, "]"))
		break;
	    if (equals(c_token, ","))
		c_token++;
	    else
		int_error(c_token, "expecting Array[size] = [x,y,...]");
	}
	c_token++;
	/* If the size is determined by the number of initializers */
	if (A[0].v.int_val == 0)
	    A[0].v.int_val = initializers;
    }

    return;
}