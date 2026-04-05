#include "Type.h"
#include "Variant.h"
#include "Method.h"

int main()
{
	simpleparser::Type* intType = simpleparser::Type::get(simpleparser::Type::BaseType::INT);
	simpleparser::Variant var = 42;
	int i = var.toInt();
	printf("Value: %d\n", i);
	return 0;
}