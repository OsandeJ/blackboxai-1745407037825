#ifndef UTILS_H
#define UTILS_H

#include <gtk/gtk.h>
#include <time.h>
#include <string.h>
#include <locale.h>

// Funções utilitárias para formatação de moeda
void formatar_kwanza(double valor, char *buffer, size_t size);

// Funções utilitárias para data e hora
void obter_data_atual(char *buffer, size_t size);
void obter_hora_atual(char *buffer, size_t size);

// Funções utilitárias para GTK
void mostrar_mensagem(GtkWindow *parent, const char *mensagem, GtkMessageType tipo);
void mostrar_erro(GtkWindow *parent, const char *mensagem);
void mostrar_sucesso(GtkWindow *parent, const char *mensagem);

// Funções de validação
gboolean validar_numero(const char *texto);
gboolean validar_telefone(const char *texto);
gboolean validar_nif(const char *texto);

// Função para inicializar configurações regionais
void inicializar_configuracoes_regionais(void);

#endif // UTILS_H
