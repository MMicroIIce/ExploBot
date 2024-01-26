#include "client_tcp/defs.h"
#include <QDebug>

void printHex(const uint8_t* data, size_t size) {
    QDebug dbg = qDebug();
    dbg << "Hexadecimal representation:";
    for (size_t i = 0; i < size; ++i) {
        dbg << "0x" << QString("%1").arg(static_cast<int>(data[i]), 2, 16, QChar('0')) << " ";
    }
    dbg.nospace() << Qt::dec << Qt::endl;
}
