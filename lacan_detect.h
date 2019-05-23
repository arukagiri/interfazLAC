#ifndef LACAN_DETECT_H
#define LACAN_DETECT_H

#include <QString>
#include <stdint.h>
#include "PC.h"

QString detect_res(uint8_t res);
QString detect_err(uint8_t err);
QString detect_var(uint8_t var);
QString detect_cmd(uint8_t cmd);
QString detect_mode(uint16_t modo);

#endif // LACAN_DETECT_H
