#ifndef PROPOSICIONES_H
#define PROPOSICIONES_H

#include <stdbool.h>

bool conjuncion(bool primero, bool segundo);

bool disyuncion(bool primero, bool segundo);

bool condicional(bool primero, bool segundo);

bool bicondicional(bool primero, bool segundo);

#endif // !PROPOSICIONES_H