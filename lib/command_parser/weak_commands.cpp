#include "command_parser.h"


#define DECLARE_WEAK_COMMAND(F)                                                                                        \
  __weak void CommandDispatcher::F() {                                                                                 \
  }


DECLARE_WEAK_COMMAND(A0);
DECLARE_WEAK_COMMAND(A1);
DECLARE_WEAK_COMMAND(A2);
DECLARE_WEAK_COMMAND(A3);


DECLARE_WEAK_COMMAND(T100);
