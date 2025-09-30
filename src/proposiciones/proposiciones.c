#include "proposiciones.h"

bool conjuncion(bool primero, bool segundo)
{
    return primero && segundo;
}

bool disyuncion(bool primero, bool segundo)
{
    return primero || segundo;
}

bool condicional(bool primero, bool segundo)
{
    if(primero && (segundo == false))
        return false;
    return true;
}

bool bicondicional(bool primero, bool segundo)
{
    return primero == segundo;
}