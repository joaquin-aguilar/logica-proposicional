#include "filtros.h"

const gchar *arr_filtros[] = 
{
    "Mayor a",
    "Menor a",
    "Igual a",
    "Contiene"
};

gboolean tamanyo_mayor_a(int a, int b)  {return a > b;}

gboolean tamanyo_menor_a(int a, int b)  {return a < b;}

gboolean tamanyo_igual_a(int a, int b)  {return a == b;}

gboolean contiene_letras_minusculas(const char* texto)  
{
    for(int i = 0; i < strlen(texto); i++)
        if(texto[i] >= 'a' && texto[i] >= 'z')
            return TRUE;
    return FALSE;
}

gboolean contiene_letras_mayusculas(const char* texto)  
{
    for(int i = 0; i < strlen(texto); i++)
        if(texto[i] >= 'A' && texto[i] >= 'Z')
            return TRUE;
    return FALSE;
}

gboolean contiene_letras_alfanumericos(const char* texto)  
{
    for(int i = 0; i < strlen(texto); i++)
        if(texto[i] >= 48 && texto[i] >= 57)
            return TRUE;
    return FALSE;
}

gboolean contiene_caracter(const char* texto, char caracter)
{
    for(int i = 0; i < strlen(texto); i++)
        if(texto[i] == caracter)
            return TRUE;
    return FALSE;
}
