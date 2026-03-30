#include "Type.h"
#include "Variant.h"
#include "Method.h"

int main()
{
	SimpleParser::Type* intType = SimpleParser::Type::get(SimpleParser::Type::BaseType::INT);
	SimpleParser::Variant var = 42;
	int i = var.toInt();
	printf("Value: %d\n", i);
	return 0;
}