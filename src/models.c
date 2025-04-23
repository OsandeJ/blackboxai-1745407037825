#include "models.h"

double calcular_preco_com_iva(double preco) {
    return preco * (1 + IVA_RATE);
}

double calcular_iva(double preco) {
    return preco * IVA_RATE;
}
