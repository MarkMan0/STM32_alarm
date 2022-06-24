#include "command_parser.h"
#include "globals.h"



void CommandDispatcher::A0() {
  DS3231::time t;
  if (0 != rtc.get_time(t)) {
    uart_->printf("Err: couldn't get time\n");
    return;
  }
  uart_->printf("Current time: ");
  uart_->printf("%02d:%02d:%02d\n", t.hour, t.min, t.sec);
  uart_->printf("Date: ");
  uart_->printf("%d.%d.%d, %s\n", t.date, t.month, t.year, t.dow_str);
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
  if (0 != rtc.get_time(t)) {
    uart_->printf("Err: Couldn't read time before set\n");
    return;
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

  if (0 != rtc.set_time(t)) {
    uart_->printf("Err: couldn't set time\n");
    return;
  }

  A0();
}



/**
 * @details Parameters:
 * N: alarm number
 * H: hour
 * M: minute
 * D: dow
 * A: 0->daily 1->onDow
 * B: enable
 */
void CommandDispatcher::A2() {
  DS3231::alarm_t alarm;

  int16_t val{};

  if (not parser_.get_parameter('N', val)) {
    uart_->printf("No N parameter\n");
    return;
  }
  if (not(val == 0 || val == 1)) {
    uart_->printf("Invalid N\n");
    return;
  }

  const int n = val;

  auto set_from_param = [](auto& what, char p) {
    int16_t val;
    if (cmd.parser_.get_parameter(p, val)) {
      what = val;
    }
  };

  if (0 != rtc.get_alarm(n, alarm)) {
    uart_->printf("Failed to get alarm");
    return;
  }

  set_from_param(alarm.hour, 'H');
  set_from_param(alarm.min, 'M');
  set_from_param(alarm.dow, 'D');
  if (parser_.get_parameter('A', val) && utils::within(val, 0, 1)) {
    alarm.alarm_type = static_cast<DS3231::alarm_t::alarm_type_t>(val);
  }
  set_from_param(alarm.en, 'B');

  if (0 != rtc.set_alarm(n, alarm)) {
    uart_->printf("Couldn't set alarm");
    return;
  }

  uart_->printf("Alarm set!\n");
  A3();
}


void CommandDispatcher::A3() {
  DS3231::alarm_t alarm;


  int16_t val{};

  if (not parser_.get_parameter('N', val)) {
    uart_->printf("No N parameter\n");
    return;
  }
  if (not(val == 0 || val == 1)) {
    uart_->printf("Invalid N\n");
    return;
  }

  const int n = val;

  if (0 != rtc.get_alarm(n, alarm)) {
    uart_->printf("Failed to get alarm %d\n", n);
    return;
  }
  uart_->printf("Alarm %d: \n\tMin: %d\n\tHour: %d\n", n, alarm.min, alarm.hour);
  uart_->printf("\tType: %d\n\tEn: %d\n", alarm.alarm_type, alarm.en);
}
