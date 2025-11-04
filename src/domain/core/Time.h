#ifndef ECOCIN_CORE_TIME_H
#define ECOCIN_CORE_TIME_H

#include <chrono>

// Definição de tipo para representar timestamps (datas/horas)
// Usa std::chrono::system_clock::time_point para precisão e portabilidade

namespace ecocin::core {

using Timestamp = std::chrono::system_clock::time_point;

} // namespace ecocin::core

#endif // ECOCIN_CORE_TIME_H
