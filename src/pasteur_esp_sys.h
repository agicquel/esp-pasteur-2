#ifndef PASTEUR_ESP_SYS_H
#define PASTEUR_ESP_SYS_H

#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include "conf_gpio.h"
#include "info_display.h"
#include "matrix_display.h"
#include "pasteur_display.h"

#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])

#define WIFI_SSID_PREFIX "Lopy"
#define WIFI_PASS "lopylopy"
#define HTTP_REST_PORT 80

#define SUBSCRIBE_ROUTE "http://10.42.31.2/subscribe?espid="
#define SUBSCRIBED_ROUTE "http://10.42.31.2/subscribed/"

enum menu_state_t
{
    MENU_STATUS,
    MENU_CONFIG,
    MENU_SCAN_WIFI,
    MENU_MAC
};

enum system_state_t
{
    IDLE,
    CONNECTING_WIFI,
    SCANNING_WIFI,
    SUBSCRIBING,
    RUNNING
};

struct configuration_t
{
  String wifi_ssid;
  String wifi_pass;
  String message;
};

class PasteurESPSys
{
private:
    menu_state_t menu_state;
    system_state_t system_state;
    configuration_t conf;
    ESP8266WebServer rest_server;
    InfoDisplay infoDisplay;
    MatrixDisplay matrixDisplay;
    PasteurDisplay pasteurDisplay;

    bool connect_wifi();
    void lopy_subscribe();
    bool lopy_subscribed();
    void handle_message_changed();
    void handle_menu();
    void EEPROM_conf_write();
    void EEPROM_conf_read();

public:
    PasteurESPSys();
    void print_bootscreen();
    void run();
};

#endif