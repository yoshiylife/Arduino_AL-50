/*
 * Copyright © 2017 Yasumitsu Yoshida aka yoshiylife
 */
#if !defined(__AL_50_H__)
#define __AL_50_H__
#include <avr/pgmspace.h>

#define SHIELD_RX_PIN 11
#define SHIELD_TX_PIN 12
#define SHIELD_STARTUP_WAIT_TIME  1000
#define SHIELD_STARTUP_TIMEOUT    1000
#define SHIELD_PROMPT_TIMEOUT     8000
#define SHIELD_ACK_TIMEOUT        8000

// 11[bytes]/SRAM
const char *SHIELD_ACTION = "\r";
const char *SHIELD_PROMPT = "\r> ";
const char *SHIELD_ACK = "\r>> ";

const char progmem_str_shield_name[] PROGMEM = "AL-50";
const char progmem_str_prompt[] PROGMEM = "--";
const char progmem_str_nl[] PROGMEM = "\n";
const char progmem_str_unknown[] PROGMEM = "unknown ";
const char progmem_str_time[] PROGMEM = "time ";
const char progmem_str_help[] PROGMEM = "help ";
const char progmem_str_Timeout[] PROGMEM = "Timeout";
const char progmem_str_Ready[] PROGMEM = "Ready";
const char progmem_str_Overflow[] PROGMEM = "Overflow ";
const char progmem_str_serial[] PROGMEM = "Serial";

const char progmem_str_mod[] PROGMEM = "mod ";
const char progmem_mod_str_00[] PROGMEM = "factory_reset";
const char progmem_mod_str_get_ver[] PROGMEM = "get_ver";
const char progmem_mod_str_02[] PROGMEM = "get_hw_deveui";
const char progmem_mod_str_get_hw_model[] PROGMEM = "get_hw_model";
const char progmem_mod_str_04[] PROGMEM = "sleep <Deep> <INTwake> <Period>";
const char progmem_mod_str_05[] PROGMEM = "set_baudrate <Baudrate>";
const char progmem_mod_str_06[] PROGMEM = "set_echo <Status>";
const char progmem_mod_str_07[] PROGMEM = "reset";
const char progmem_mod_str_08[] PROGMEM = "save";

const char progmem_mod_str_set_echo_off[] PROGMEM = "set_echo off";

const char* const progmem_cmd_list_mod[] PROGMEM =
{
  progmem_mod_str_00,
  progmem_mod_str_get_ver,
  progmem_mod_str_02,
  progmem_mod_str_get_hw_model,
  progmem_mod_str_04,
  progmem_mod_str_05,
  progmem_mod_str_06,
  progmem_mod_str_07,
  progmem_mod_str_08,
  NULL
};

const char progmem_str_lorawan[] PROGMEM = "lorawan ";
const char progmem_cmd_10[] PROGMEM = "factory_reset";
const char progmem_cmd_11[] PROGMEM = "join  {abp|otaa}";
const char progmem_cmd_12[] PROGMEM = "get_join_status";
const char progmem_cmd_13[] PROGMEM = "get_ch_plan";
const char progmem_cmd_14[] PROGMEM = "tx <Type> <PortNum> <Data>";
const char progmem_cmd_15[] PROGMEM = "set_dr <DataRate>";
const char progmem_cmd_16[] PROGMEM = "set_adr <State>";
const char progmem_cmd_17[] PROGMEM = "set_txretry <RetryCount>";
const char progmem_cmd_18[] PROGMEM = "set_linkchk";
const char progmem_cmd_19[] PROGMEM = "set_battery <BatteryLevel>";
const char progmem_cmd_1a[] PROGMEM = "save";
const char progmem_cmd_1b[] PROGMEM = "get_devaddr";
const char progmem_lorawan_str_get_deveui[] PROGMEM = "get_deveui";
const char progmem_cmd_1d[] PROGMEM = "get_dr";
const char progmem_cmd_1e[] PROGMEM = "get_pwr";
const char progmem_cmd_1f[] PROGMEM = "get_adr";
const char progmem_cmd_1g[] PROGMEM = "get_txretry";
const char progmem_cmd_1h[] PROGMEM = "get_rxdelay";
const char progmem_cmd_1i[] PROGMEM = "get_rx2";
const char progmem_cmd_1j[] PROGMEM = "get_ch_status <ChannelID>";
const char progmem_cmd_1k[] PROGMEM = "get_dc_ctl";
const char progmem_cmd_1l[] PROGMEM = "get_join_ch";
const char progmem_cmd_1m[] PROGMEM = "get_upcnt";
const char progmem_cmd_1n[] PROGMEM = "get_downcnt";
const char progmem_cmd_1o[] PROGMEM = "get_class";
const char progmem_cmd_1p[] PROGMEM = "get_pwr_table <Index>";
const char progmem_cmd_1q[] PROGMEM = "get_max_payload_table <DRIndex>";
const char progmem_cmd_1r[] PROGMEM = "get_as923_para";
const char progmem_cmd_1x[] PROGMEM = "get_dl_freq <ChannelID>";
const char progmem_cmd_1y[] PROGMEM = "get_battery";

const char* const progmem_cmd_list_lorawan[] PROGMEM =
{
  progmem_cmd_10,
  progmem_cmd_11,
  progmem_cmd_12,
  progmem_cmd_13,
  progmem_cmd_14,
  progmem_cmd_15,
  progmem_cmd_16,
  progmem_cmd_17,
  progmem_cmd_18,
  progmem_cmd_19,
  progmem_cmd_1a,
  progmem_cmd_1b,
  progmem_lorawan_str_get_deveui,
  progmem_cmd_1d,
  progmem_cmd_1e,
  progmem_cmd_1f,
  progmem_cmd_1g,
  progmem_cmd_1h,
  progmem_cmd_1i,
  progmem_cmd_1j,
  progmem_cmd_1k,
  progmem_cmd_1l,
  progmem_cmd_1m,
  progmem_cmd_1n,
  progmem_cmd_1o,
  progmem_cmd_1p,
  progmem_cmd_1q,
  progmem_cmd_1r,
  progmem_cmd_1x,
  progmem_cmd_1y,
  NULL
};

struct SHIELD_COMMAND_LIST
{
  const char *progmem_type;
  const char * const *progmem_list;
}
SHIELD_COMMAND_LIST[] =
{
  { progmem_str_mod,      progmem_cmd_list_mod },
  { progmem_str_lorawan,  progmem_cmd_list_lorawan }
};

struct SHIELD_COMMAND
{
    const char *type;
    const char *cmd;
};

struct SHIELD_COMMAND SHIELD_COMMAND_ECHO_OFF = {
  progmem_str_mod,progmem_mod_str_set_echo_off
};

struct SHIELD_COMMAND SHIELD_COMMAND_STARTUP[] = {
    {progmem_str_mod,progmem_mod_str_get_hw_model},
    {progmem_str_mod,progmem_mod_str_get_ver},
    {progmem_str_lorawan,progmem_lorawan_str_get_deveui}
};

#endif  /* !defined(__AL_50_H__) */
