#include "arbol_evaluacion.h"
#include <glib.h>
#include <gtk/gtk.h>
#include <ctype.h>
#include <string.h>
#include "../gui/ventana_principal.h"

typedef enum 
{
    NODO_OPERADOR,
    NODO_PROPOSICION
} TipoNodo;

typedef struct NodoLogico 
{
    TipoNodo tipo;
    union 
    {
        struct 
        {
            char operador;
            struct NodoLogico *izq;
            struct NodoLogico *der;
        } operador;
        char letra;
    };
} NodoLogico;

static gboolean es_operador(char c)
{
    return c == '~' || c == '^' || c == 'v' || c == '>' || c == '-' || c == '(' || c == ')';
}

static gboolean obtener_valor_letra(GList *proposiciones, char letra) 
{
    for (GList *l = proposiciones; l != NULL; l = l->next) 
    {
        Proposicion *p = l->data;
        if (p->letra == letra) return p->valor;
    }
    g_warning("Letra '%c' no encontrada", letra);
    return FALSE;
}

static NodoLogico* crear_nodo_proposicion(char letra) 
{
    NodoLogico *nodo = g_new(NodoLogico, 1);
    nodo->tipo = NODO_PROPOSICION;
    nodo->letra = letra;
    return nodo;
}

static NodoLogico* crear_nodo_operador(char operador, NodoLogico *izq, NodoLogico *der) 
{
    NodoLogico *nodo = g_new(NodoLogico, 1);
    nodo->tipo = NODO_OPERADOR;
    nodo->operador.operador = operador;
    nodo->operador.izq = izq;
    nodo->operador.der = der;
    return nodo;
}

static int prioridad(char op) 
{
    switch (op) 
    {
        case '~': return 3;
        case '^': return 2;
        case 'v': return 1;
        case '>': return 0;
        default: return -1;
    }
}

static NodoLogico* construir_arbol(const gchar *expr) 
{
    GList *nodos = NULL;
    GList *operadores = NULL;

    for (int i = 0; expr[i]; i++) 
    {
        char c = expr[i];
        if (isspace(c)) continue;

        if (isalpha(c) && !es_operador(c)) 
            nodos = g_list_prepend(nodos, crear_nodo_proposicion(c));

        else if (c == '(') 
            operadores = g_list_prepend(operadores, GINT_TO_POINTER(c));

        else if (c == ')') 
        {
            while (operadores && GPOINTER_TO_INT(operadores->data) != '(') 
            {
                char op = GPOINTER_TO_INT(operadores->data);
                operadores = g_list_delete_link(operadores, operadores);

                NodoLogico *der = NULL;
                NodoLogico *izq = NULL;

                if (op == '~') 
                {
                    izq = g_list_nth_data(nodos, 0);
                    nodos = g_list_delete_link(nodos, nodos);
                } 
                else 
                {
                    der = g_list_nth_data(nodos, 0);
                    nodos = g_list_delete_link(nodos, nodos);
                    izq = g_list_nth_data(nodos, 0);
                    nodos = g_list_delete_link(nodos, nodos);
                }

                nodos = g_list_prepend(nodos, crear_nodo_operador(op, izq, der));
            }
            if (operadores) operadores = g_list_delete_link(operadores, operadores);
        } 
        else 
        {
            if (c == '-' && expr[i+1] == '>') 
            {
                c = '>';
                i++;
            }
            while (operadores && prioridad(GPOINTER_TO_INT(operadores->data)) >= prioridad(c)) 
            {
                char op = GPOINTER_TO_INT(operadores->data);
                operadores = g_list_delete_link(operadores, operadores);

                NodoLogico *der = NULL;
                NodoLogico *izq = NULL;

                if (op == '~') 
                {
                    izq = g_list_nth_data(nodos, 0);
                    nodos = g_list_delete_link(nodos, nodos);
                } 
                else 
                {
                    der = g_list_nth_data(nodos, 0);
                    nodos = g_list_delete_link(nodos, nodos);
                    izq = g_list_nth_data(nodos, 0);
                    nodos = g_list_delete_link(nodos, nodos);
                }

                nodos = g_list_prepend(nodos, crear_nodo_operador(op, izq, der));
            }
            operadores = g_list_prepend(operadores, GINT_TO_POINTER(c));
        }
    }

    while (operadores) 
    {
        char op = GPOINTER_TO_INT(operadores->data);
        operadores = g_list_delete_link(operadores, operadores);

        NodoLogico *der = NULL;
        NodoLogico *izq = NULL;

        if (op == '~') 
        {
            izq = g_list_nth_data(nodos, 0);
            nodos = g_list_delete_link(nodos, nodos);
        } else 
        {
            der = g_list_nth_data(nodos, 0);
            nodos = g_list_delete_link(nodos, nodos);
            izq = g_list_nth_data(nodos, 0);
            nodos = g_list_delete_link(nodos, nodos);
        }

        nodos = g_list_prepend(nodos, crear_nodo_operador(op, izq, der));
    }

    NodoLogico *raiz = g_list_nth_data(nodos, 0);
    g_list_free(nodos);
    return raiz;
}

static gboolean evaluar_arbol(NodoLogico *nodo, GList *proposiciones) 
{
    if (!nodo) 
    {
        g_warning("Nodo lógico nulo");
        return FALSE;
    }

    if (nodo->tipo == NODO_PROPOSICION) 
        return obtener_valor_letra(proposiciones, nodo->letra);


    gboolean izq = evaluar_arbol(nodo->operador.izq, proposiciones);
    gboolean der = nodo->operador.der ? evaluar_arbol(nodo->operador.der, proposiciones) : FALSE;

    switch (nodo->operador.operador) 
    {
        case '~': return !izq;
        case '^': return izq && der;
        case 'v': return izq || der;
        case '>': return !izq || der;
        default:
            g_warning("Operador desconocido: %c", nodo->operador.operador);
            return FALSE;
    }
}

gboolean evaluar_expresion_logica(const gchar *expr, GList *proposiciones) 
{
    NodoLogico *arbol = construir_arbol(expr);
    if (!arbol) 
    {
        g_warning("No se pudo construir el árbol lógico");
        return FALSE;
    }
    gboolean resultado = evaluar_arbol(arbol, proposiciones);
    return resultado;
}

static int contador_nodos_etiquetados = 0;

static gchar* generar_nombre_nodo_etiquetado(const gchar *contenido) 
{
    return g_strdup_printf("nodo_etq_%d", contador_nodos_etiquetados++);
}

static gchar* generar_etiqueta_completa(const NodoLogico *nodo) 
{
    if (!nodo) return g_strdup("NULL");

    if (nodo->tipo == NODO_PROPOSICION) {
        return g_strdup_printf("%c", nodo->letra);
    }

    gchar *izq = generar_etiqueta_completa(nodo->operador.izq);
    gchar *der = nodo->operador.der ? generar_etiqueta_completa(nodo->operador.der) : NULL;

    gchar *resultado = NULL;
    switch (nodo->operador.operador) {
        case '~':
            resultado = g_strdup_printf("~%s", izq);
            break;
        case '^':
            resultado = g_strdup_printf("(%s ∧ %s)", izq, der);
            break;
        case 'v':
            resultado = g_strdup_printf("(%s ∨ %s)", izq, der);
            break;
        case '>':
            resultado = g_strdup_printf("(%s → %s)", izq, der);
            break;
        default:
            resultado = g_strdup("?");
    }

    g_free(izq);
    if (der) g_free(der);
    return resultado;
}

static void escribir_dot_etiquetado(FILE *f, NodoLogico *nodo, const gchar *padre) 
{
    if (!nodo) return;

    gchar *etiqueta = generar_etiqueta_completa(nodo);
    gchar *nombre = generar_nombre_nodo_etiquetado(etiqueta);

    fprintf(f, "    %s [label=\"%s\"];\n", nombre, etiqueta);
    if (padre)
        fprintf(f, "    %s -> %s;\n", padre, nombre);

    if (nodo->tipo == NODO_OPERADOR) {
        escribir_dot_etiquetado(f, nodo->operador.izq, nombre);
        if (nodo->operador.der)
            escribir_dot_etiquetado(f, nodo->operador.der, nombre);
    }

    g_free(etiqueta);
    g_free(nombre);
}

void exportar_arbol_dot_etiquetado(const gchar *expr, const gchar *ruta_archivo) 
{
    contador_nodos_etiquetados = 0;
    NodoLogico *arbol = construir_arbol(expr);
    if (!arbol) 
    {
        g_warning("No se pudo construir el árbol lógico para exportar");
        return;
    }

    gchar *directorio = g_path_get_dirname(ruta_archivo);
    GFile *dir_file = g_file_new_for_path(directorio);
    if (!g_file_query_exists(dir_file, NULL)) 
    {
        if (!g_file_make_directory_with_parents(dir_file, NULL, NULL)) 
        {
            g_warning("No se pudo crear el directorio: %s", directorio);
            g_object_unref(dir_file);
            g_free(directorio);
            return;
        }
    }
    g_object_unref(dir_file);
    g_free(directorio);

    FILE *f = fopen(ruta_archivo, "w");
    if (!f) 
    {
        g_warning("No se pudo abrir el archivo DOT: %s", ruta_archivo);
        return;
    }

    fprintf(f, "digraph ArbolFormacion {\n");
    fprintf(f, "    node [shape=plaintext, fontname=\"Arial\"];\n\n");

    escribir_dot_etiquetado(f, arbol, NULL);

    fprintf(f, "}\n");
    fclose(f);
    g_print("Árbol etiquetado exportado a DOT: %s\n", ruta_archivo);
}
