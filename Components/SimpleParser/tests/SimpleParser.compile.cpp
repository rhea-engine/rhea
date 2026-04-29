/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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