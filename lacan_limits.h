#ifndef LACAN_LIMITS_H
#define LACAN_LIMITS_H

#include "lacan_limits_gen.h"
#include "comandar.h"
#include "consultar.h"
#include "mainwindow.h"
#include "estadored.h"
#include "enviar_mensaje.h"
#include "gen_eolico.h"

using namespace std;

void set_limits_gen(Gen_Eolico *gen_win);
void set_limits_gen(Comandar *win, uint16_t var);
void set_limits_gen(Enviar_Mensaje *win, uint16_t var);

#endif // LACAN_LIMITS_H
