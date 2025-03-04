 */
static void php_wddx_pop_element(void *user_data, const XML_Char *name)
{
	st_entry 			*ent1, *ent2;
	wddx_stack 			*stack = (wddx_stack *)user_data;
	HashTable 			*target_hash;
	zend_class_entry 	*pce;
	zval				obj;

/* OBJECTS_FIXME */
	if (stack->top == 0) {
		return;
	}

	if (!strcmp((char *)name, EL_STRING) || !strcmp((char *)name, EL_NUMBER) ||
		!strcmp((char *)name, EL_BOOLEAN) || !strcmp((char *)name, EL_NULL) ||
	  	!strcmp((char *)name, EL_ARRAY) || !strcmp((char *)name, EL_STRUCT) ||
		!strcmp((char *)name, EL_RECORDSET) || !strcmp((char *)name, EL_BINARY) ||
		!strcmp((char *)name, EL_DATETIME)) {
		wddx_stack_top(stack, (void**)&ent1);

		if (Z_TYPE(ent1->data) == IS_UNDEF) {
			if (stack->top > 1) {
				stack->top--;
				efree(ent1);
			} else {
				stack->done = 1;
			}
			return;
		}

		if (!strcmp((char *)name, EL_BINARY)) {
			zend_string *new_str = NULL;
			
			if (ZSTR_EMPTY_ALLOC() != Z_STR(ent1->data)) {
				new_str = php_base64_decode(
					(unsigned char *)Z_STRVAL(ent1->data), Z_STRLEN(ent1->data));
			}

			zval_ptr_dtor(&ent1->data);
			if (new_str) {
				ZVAL_STR(&ent1->data, new_str);
			} else {
				ZVAL_EMPTY_STRING(&ent1->data);
			}
		}

		/* Call __wakeup() method on the object. */
		if (Z_TYPE(ent1->data) == IS_OBJECT) {
			zval fname, retval;

			ZVAL_STRING(&fname, "__wakeup");

			call_user_function_ex(NULL, &ent1->data, &fname, &retval, 0, 0, 0, NULL);

			zval_ptr_dtor(&fname);
			zval_ptr_dtor(&retval);
		}

		if (stack->top > 1) {
			stack->top--;
			wddx_stack_top(stack, (void**)&ent2);

			/* if non-existent field */
			if (Z_ISUNDEF(ent2->data)) {
				zval_ptr_dtor(&ent1->data);
				efree(ent1);
				return;
			}

			if (Z_TYPE(ent2->data) == IS_ARRAY || Z_TYPE(ent2->data) == IS_OBJECT) {
				target_hash = HASH_OF(&ent2->data);

				if (ent1->varname) {
					if (!strcmp(ent1->varname, PHP_CLASS_NAME_VAR) &&
						Z_TYPE(ent1->data) == IS_STRING && Z_STRLEN(ent1->data) &&
						ent2->type == ST_STRUCT && Z_TYPE(ent2->data) == IS_ARRAY) {
						zend_bool incomplete_class = 0;

						zend_str_tolower(Z_STRVAL(ent1->data), Z_STRLEN(ent1->data));
						zend_string_forget_hash_val(Z_STR(ent1->data));
						if ((pce = zend_hash_find_ptr(EG(class_table), Z_STR(ent1->data))) == NULL) {
							incomplete_class = 1;
							pce = PHP_IC_ENTRY;
						}

						if (pce != PHP_IC_ENTRY && (pce->serialize || pce->unserialize)) {
							zval_ptr_dtor(&ent2->data);
							ZVAL_UNDEF(&ent2->data);
							php_error_docref(NULL, E_WARNING, "Class %s can not be unserialized", Z_STRVAL(ent1->data));
						} else {
							/* Initialize target object */
							object_init_ex(&obj, pce);

							/* Merge current hashtable with object's default properties */
							zend_hash_merge(Z_OBJPROP(obj),
											Z_ARRVAL(ent2->data),
											zval_add_ref, 0);

							if (incomplete_class) {
								php_store_class_name(&obj, Z_STRVAL(ent1->data), Z_STRLEN(ent1->data));
							}

							/* Clean up old array entry */
							zval_ptr_dtor(&ent2->data);

							/* Set stack entry to point to the newly created object */
							ZVAL_COPY_VALUE(&ent2->data, &obj);
						}

						/* Clean up class name var entry */
						zval_ptr_dtor(&ent1->data);
					} else if (Z_TYPE(ent2->data) == IS_OBJECT) {
						zend_class_entry *old_scope = EG(scope);

						EG(scope) = Z_OBJCE(ent2->data);
						add_property_zval(&ent2->data, ent1->varname, &ent1->data);
						if Z_REFCOUNTED(ent1->data) Z_DELREF(ent1->data);
						EG(scope) = old_scope;
					} else {
						zend_symtable_str_update(target_hash, ent1->varname, strlen(ent1->varname), &ent1->data);
					}
					efree(ent1->varname);
				} else	{
					zend_hash_next_index_insert(target_hash, &ent1->data);
				}
			}
			efree(ent1);
		} else {
			stack->done = 1;
		}
	} else if (!strcmp((char *)name, EL_VAR) && stack->varname) {
		efree(stack->varname);
		stack->varname = NULL;
	} else if (!strcmp((char *)name, EL_FIELD)) {
		st_entry *ent;
		wddx_stack_top(stack, (void **)&ent);
		efree(ent);
		stack->top--;
	}