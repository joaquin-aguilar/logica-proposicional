#ifndef FILTROS_H
#define FILTROS_H

#include <glib.h>

extern const gchar *arr_filtros[];

enum 
{
    MAYOR_A,
    MENOR_A,
    IGUAL_A,
    CONTIENE_A,
    CONTIENE_MINUSCULAS,
    CONTIENE_MAYUSCULAS,
    CONTIENE_NUMERICOS,
    CONTIENE_CARACTERES_ESPECIALES
};

gboolean tamanyo_mayor_a(int a, int b); // P=(∣A∣ > b)
gboolean tamanyo_menor_a(int a, int b); // P=(∣A∣ < b)
gboolean tamanyo_igual_a(int a, int b); // P=(∣A∣ = b)
gboolean contiene_letras_minusculas(const char* texto); //  ∃ x ∈ Texto,  (x = a) v (x = b) ... (x = z)
gboolean contiene_letras_mayusculas(const char* texto); //  ∃ x ∈ Texto,  (x = A) v (x = B) ... (x = Z)
gboolean contiene_numericos(const char* texto);  //  ∃ x ∈ Texto,  (x = 0) v (x = 1) ... (x = 9)
gboolean contiene_caracteres_especiales(const char* texto); //  ∃ x ∈ Texto,  (x = @) v (x = /) ... (x = A)
gboolean contiene_caracter(const char* texto, char caracter); //  ∃ x ∈ Texto,  (x = caracter)

#endif
