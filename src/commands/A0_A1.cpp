#include "command_parser.h"
#include "globals.h"



void CommandDispatcher::A0() {
  DS3231::time t;
  if (not rtc.get_time(t)) {
    uart2.printf("Err: couldn't get time\n");
  }
  uart2.printf("Current time: ");
  uart2.printf("%02d:%02d:%02d\n", t.hour, t.min, t.sec);
  uart2.printf("Date: ");
  uart2.printf("%d.%d.%d, %s\n", t.date, t.month, t.year, t.dow_str);
}

/**
 * @details Command parameter:
 * H: hour
 * M: minute
 * S: seconds
 * A: date
 * B: month
 * C: year
 * D: dow
 */
void CommandDispatcher::A1() {
  DS3231::time t;
  if (not rtc.get_time(t)) {
    uart2.printf("Err: Couldn't read time before set\n");
  }

  auto set_from_param = [](auto& what, char p) {
    int16_t val;
    if (cmd.parser_.get_parameter(p, val)) {
      what = val;
    }
  };

  set_from_param(t.hour, 'H');
  set_from_param(t.min, 'M');
  set_from_param(t.sec, 'S');
  set_from_param(t.date, 'A');
  set_from_param(t.month, 'B');
  set_from_param(t.year, 'C');
  set_from_param(t.dow, 'D');

  if (not rtc.set_time(t)) {
    uart2.printf("Err: couldn't set time\n");
  }

  A0();
}
