/*
   Copyright © 2017 Yasumitsu Yoshida aka yoshiylife
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <Process.h>
#include "AL-50.h"

// 124 [bytes]/SRAM

#define ALIVE_INTERVAL  1000

// Shield Serial only one
SoftwareSerial Shield(SHIELD_RX_PIN, SHIELD_TX_PIN); // 82 +31 [bytes]/SRAM

#define BUFFER_SIZE 127
#define DEVICE_Serial 0
#define DEVICE_Shield 1
struct buffer {
  char dev; // 0:Serial 1:Shield
  unsigned char len;
  char dat[BUFFER_SIZE + 1];
};

// 297 [bytes]/SRAM

// the LED on/off function with specified interval(milli seconds)
void my_alive(unsigned long interval)
{
  static unsigned long stamp = 0;
  static char state = 0;
  unsigned long now = millis();

  if (now - stamp > interval) {
    state = (state ? 0 : 1);
    // turn the LED on (HIGH is the voltage level)
    // turn the LED off by making the voltage LOW
    digitalWrite(LED_BUILTIN, (state ? HIGH : LOW));
  } else {
    stamp = now;
  }
}

char *my_alloc(char **p, size_t size)
{
  char *ret;
  if (p && *p) free(*p);
  ret = (char *)malloc(size);
  if (p) *p = ret;
  return ret;
}

void *my_free(char **p)
{
  if (p && *p) {
    free(*p);
    *p = NULL;
  }
}

char *my_string(char **p, const char *progmem_str)
{
  static char *reuse = NULL;
  if (p == NULL) p = &reuse;
  my_alloc(p, strlen_P(progmem_str) + 1);
  strcpy_P(*p, progmem_str);
  return *p;
}

void my_display(const char *progmem_str)
{
  Serial.print(my_string(NULL, progmem_str_prompt));
  Serial.print(my_string(NULL, progmem_str));
}

char *my_atime(unsigned long now)
{
  static char *buf;
  my_alloc(&buf, 15);
  char *p = buf;
  unsigned long mi = now % 1000; now /= 1000;
  unsigned long ss = now % 60; now /= 60;
  unsigned long mm = now % 60; now /= 60;
  unsigned long hh = now % 60;

  now = 100;
  now += hh; now *= 1000;
  now += mm; now *= 1000;
  now += ss;

  ultoa(now, p, 10); p += 3; *p = ':'; p += 3; *p = ':'; p += 3;
  ultoa(mi + 1000, p, 10); *p = '.';

  return buf + 1;
}

char *buf_receive(struct buffer *buf)
{
  my_alive(ALIVE_INTERVAL);
  char *ret = NULL;
  for (;;) {
    boolean available;
    char one;
    switch (buf->dev) {
      case DEVICE_Serial:
        if (available = Serial.available()) one = Serial.read();
        break;
      case DEVICE_Shield:
        if (available = Shield.available()) one = Shield.read();
        break;
      default: // Ignore
        available = false;
        break;
    }
    if (!available) break;
    if (buf->len < BUFFER_SIZE) {
      if (ret == NULL) ret = buf->dat + buf->len;
      buf->dat[buf->len ++] = one;
    } else {
      my_display(progmem_str_Overflow);
      Serial.write('('); Serial.write(buf->dev + '0'); Serial.write(')'); Serial.write(':');
      Serial.println(buf->dat);
      buf->len = 0;
      my_display(progmem_str_Ready);
      Shield.print(SHIELD_ACTION);
    }
    buf->dat[buf->len] = '\0';
  }
  return ret;
}

void buf_display(struct buffer *buf)
{
  if (buf->len) {
    Serial.print(buf->dat);
    buf->len = 0;
  }
}

void buf_clear(struct buffer *buf)
{
  buf->dat[buf->len = 0] = 0;
}

// the wait to receive specified prompt, and return position of prompt
char *my_wait(struct buffer *shield, const char *prompt, unsigned long timeout)
{
  unsigned long limit = millis() + timeout;
  char *pos = NULL;
  while (millis() <= limit) {
    if (buf_receive(shield)) {
      pos = strstr(shield->dat, prompt);
      if (pos != NULL) break;
    }
    my_alive(1000); // LED on/off, first argumen is interval(milli second)
  }

  if (pos == NULL) {
    my_display(progmem_str_Timeout);
    Serial.write('('); Serial.write(shield->dev + '0'); Serial.write(','); Serial.print(timeout); Serial.write(')');
    Serial.write(':'); Serial.println(prompt);
  }
  return pos;
}

void cmd_default(char *args) {
  Serial.print(my_string(NULL, progmem_str_nl));
  Serial.print(my_string(NULL, progmem_str_prompt));
  Serial.print(my_string(NULL, progmem_str_unknown));
  Serial.print(args);
}

int cmd_echo(char *args) {
  Serial.print(my_string(NULL, progmem_str_nl));
  Serial.print(my_string(NULL, progmem_str_prompt));
  Serial.print(args);

  return 0;
}

int cmd_time(char *args) {
  Serial.print(my_string(NULL, progmem_str_nl));
  Serial.print(my_string(NULL, progmem_str_prompt));
  Serial.print(my_string(NULL, progmem_str_time));

  char *str = my_atime(millis());
  Serial.print(str);
  Serial.print(args);

  return 0;
}

//const char *my_strstr(const char *stack, const char *needle)
//{
//  const char *p = NULL;
//  size_t len = strlen(needle);
//  Serial.print("len=");Serial.println(len);
//  if (len) {
//    do {
//      if ((p = strchr(stack, *needle)) == NULL) break;
//      if (strncmp(p, needle, len) == 0) break;
//      else {
//        stack = p + 1;
//        p = NULL;
//      }
//    } while (*stack != '\0');
//  } else p = stack;
//  Serial.print("p=");Serial.println(p);
//  return p;
//}

// print usage of command
int cmd_help(char *args) {
  Serial.print(my_string(NULL, progmem_str_nl));
  Serial.print(my_string(NULL, progmem_str_prompt));
  Serial.print(my_string(NULL, progmem_str_help));
  Serial.print(args);

  int n = sizeof(SHIELD_COMMAND_LIST) / sizeof(struct SHIELD_COMMAND_LIST);
  for (int i = 0; i < n; i ++) {
    struct SHIELD_COMMAND_LIST *cmds = SHIELD_COMMAND_LIST + i;
    const char * const *list = cmds->progmem_list;
    const char *progmem_str = NULL;
    int type_len = strlen_P(cmds->progmem_type);
    char *type = (char *)malloc(type_len + 1);
    strcpy_P(type, cmds->progmem_type);
    while ((progmem_str = (const char *)pgm_read_word(list ++)) != NULL) {
      int str_len = strlen_P(progmem_str);
      char *usage = (char *)malloc(type_len + str_len + 3);
      char *p = usage;
      *(p ++) = '\n';
      strcpy(p, type); p += type_len;
      strcpy_P(p, progmem_str); p += str_len;
      *p = '\0';
      if (strcasestr(usage, args)) {
        Serial.print(usage);
      }
      free(usage);
    }
    free(type);
  }

  return 0;
}

// wait for received string
// args: [prompt [timeout]]
int cmd_wait(struct buffer *serial, struct buffer *shield, char *args) {
  const char *prompt = SHIELD_PROMPT; // default
  unsigned int timeout = SHIELD_ACK_TIMEOUT; //default

  if (*args != '\0') {
    prompt = args;
    char *pos = strchr(args, ' ');
    if (pos) {
      *pos ++ = '\0';
      timeout = int(pos);
    }
  }

  char *res = my_wait(shield, prompt, timeout);
  Serial.write(res);
  return res == NULL;
}

void shield_command(struct buffer *serial, struct buffer *shield, char *type, char *str, int quiet)
{
  buf_clear(shield);
  Shield.print(type);
  Shield.print(str);
  Shield.print(SHIELD_ACTION);
  char *ack = my_wait(shield, SHIELD_ACK, SHIELD_ACK_TIMEOUT);
  //Serial.print("[debug ack="); Serial.print(ack); Serial.print("]");
  char *prompt = my_wait(shield, SHIELD_PROMPT, SHIELD_PROMPT_TIMEOUT);
  //Serial.print("[debug prompt="); Serial.print(prompt); Serial.print("]");
  if (ack && prompt) {
    if (!quiet) {
      //*prompt = '\0';
      //ack += sizeof(SHIELD_PROMPT) + 2;
      Serial.print(ack);
    }
    buf_clear(shield);
  } else {
    buf_display(shield);
  }
}

int my_shell(struct buffer *serial, buffer *shield, char *cmdline, char *options)
{
  int ret = 0;
  if (cmdline == NULL) return ret;
  char *cmd_spos = cmdline;
  char *str_end = cmdline + strlen(cmdline);
  char *cmd_epos, *res;
  char prompt = false;
  if (cmd_spos == str_end) {
    prompt = true;
  } else {
    if (*cmd_spos != '#') {
      do {
        buf_display(shield);
        cmd_epos = strchr(cmd_spos, ';');
        if (cmd_epos == NULL) cmd_epos = str_end;
        *cmd_epos = '\0';
        while (*cmd_spos == ' ' || *cmd_spos == '\t') cmd_spos ++;
        //Serial.print("[debug cmd_spos="); Serial.print(cmd_spos); Serial.print("]");
        switch (*cmd_spos) {
          case '#': ret = 0; break;
          case '?': ret = cmd_help(++ cmd_spos); prompt = true; break;
          case '-': ret = cmd_echo(++ cmd_spos); prompt = true; break;
          case '.': ret = cmd_time(++ cmd_spos); prompt = true; break;
          default: shield_command(serial, shield, NULL, cmd_spos, 0); prompt = false; break;
        }
        if (ret) break;
        my_alive(1000); // LED on/off, first argumen is interval(milli second)
        cmd_spos = cmd_epos + 1;
      } while (cmd_spos < str_end);
    }
  }
  if (prompt) {
    Shield.print(SHIELD_ACTION);
    my_wait(shield, SHIELD_PROMPT, SHIELD_PROMPT_TIMEOUT);
    buf_display(shield);
  }
  return ret;
}

void doit_console(struct buffer *serial, struct buffer *shield)
{
  char *res = buf_receive(serial);
  if (res) {
    char *eol = strchr(res, '\r');
    if (eol) {
      *eol = '\0';
      char *cmdline = strdup(serial->dat);
      buf_clear(serial);
      my_shell(serial, shield, cmdline, NULL);
      free(cmdline);
    }
  }
}

void doit_shield(struct buffer *serial, struct buffer *shield)
{
  buf_receive(shield);
  buf_display(shield);
}

void doit_setup(struct buffer *serial, struct buffer *shield)
{
  char *type = NULL;
  char *cmd = NULL;

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Shield.begin(9600);
  delay(SHIELD_STARTUP_WAIT_TIME);

  // first contact device for echo off
  my_string(&type, SHIELD_COMMAND_ECHO_OFF.type);
  my_string(&cmd, SHIELD_COMMAND_ECHO_OFF.cmd);
  shield_command(serial, shield, type, cmd, 1);

  // startup cmd for verbose message
  int n = sizeof(SHIELD_COMMAND_STARTUP) / sizeof(struct SHIELD_COMMAND);
  for (int i = 0; i < n; i ++) {
    my_string(&type, SHIELD_COMMAND_STARTUP[i].type);
    my_string(&cmd, SHIELD_COMMAND_STARTUP[i].cmd);
    shield_command(serial, shield, type, cmd, 0);
  }

  my_free(&type);
  my_free(&cmd);

  my_display(progmem_str_Ready);
  Shield.print(SHIELD_ACTION);
}

struct buffer Serial_RecvBuf = {.dev = DEVICE_Serial, .len = 0};
struct buffer Shield_RecvBuf = {.dev = DEVICE_Shield, .len = 0};

// 476 [bytes]/SRAM

// the setup function runs once when you press reset or power the board
void setup()
{
  doit_setup(&Serial_RecvBuf, &Shield_RecvBuf);
}

// the loop function runs over and over again forever
void loop()
{
  doit_console(&Serial_RecvBuf, &Shield_RecvBuf);
  doit_shield(&Shield_RecvBuf, &Shield_RecvBuf);
}

// 608 [bytes]/SRAM

// End of file.
