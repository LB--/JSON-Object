#include "Common.h"


bool Extension::OnError()
{
	return true;
}
bool Extension::IsString()
{
	return current->type == json_string;
}
bool Extension::IsInteger()
{
	return current->type == json_integer;
}
bool Extension::IsDouble()
{
	return current->type == json_double;
}
bool Extension::IsObject()
{
	return current->type == json_object;
}
bool Extension::IsArray()
{
	return current->type == json_array;
}
bool Extension::IsBoolean()
{
	return current->type == json_boolean;
}
bool Extension::IsNull()
{
	return current->type == json_null;
}
bool Extension::IsTrue()
{
	return IsBoolean() && current->u.boolean;
}
