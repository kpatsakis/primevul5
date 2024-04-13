ObjectSetInUse(
	       OBJECT          *object
	       )
{
    object->attributes.occupied = SET;
}