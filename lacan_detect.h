#ifndef LACAN_DETECT_H
#define LACAN_DETECT_H

#include <QString>
#include <stdint.h>
#include "PC.h"
#include <QDebug>

QString detect_res(uint8_t res);
QString detect_err(uint8_t err);
QString detect_var(uint8_t var);
QString detect_cmd(uint8_t cmd);
QString detect_mode(uint8_t modo);
float lacan_data_float(char b0, char b1, char b2, char b3);

#endif // LACAN_DETECT_H
