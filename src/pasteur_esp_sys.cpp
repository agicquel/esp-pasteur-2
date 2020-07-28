#include "pasteur_esp_sys.h"

PasteurESPSys::PasteurESPSys() : rest_server(HTTP_REST_PORT), infoDisplay(), matrixDisplay(), pasteurDisplay()
{
    EEPROM.begin(512);
    EEPROM_conf_read();
    menu_state = MENU_CONFIG;
    system_state = IDLE;
    if (conf.wifi_ssid != NULL && conf.wifi_ssid != "")
        system_state = CONNECTING_WIFI;
}

bool PasteurESPSys::connect_wifi()
{
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    float sec_timeout = 10.0;
    float sec_time = 0.0;
    String loading = "";
    infoDisplay.set_text("Connexion", "ssid : " + conf.wifi_ssid, "");
    WiFi.begin(conf.wifi_ssid, conf.wifi_pass);
    while (WiFi.status() != WL_CONNECTED && sec_time < sec_timeout)
    {
        delay(500);
        sec_time += 0.5;
        loading += " .";
        infoDisplay.set_text("Connexion", "ssid : " + conf.wifi_ssid, loading);
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        infoDisplay.set_text("Connexion", "ssid : " + conf.wifi_ssid, "connecte.");
        system_state = SUBSCRIBING;
        delay(1500);
    }
    else
    {
        infoDisplay.set_text("Connexion", "ssid : " + conf.wifi_ssid, "connexion impossible.");
        system_state = IDLE;
        delay(1500);
    }
    return WiFi.status() == WL_CONNECTED;
}

void PasteurESPSys::lopy_subscribe()
{
    HTTPClient http;
    String espid = WiFi.macAddress();
    espid.replace(":", "");
    String route = SUBSCRIBE_ROUTE + espid;
    http.begin(route);
    http.POST("");
    http.end();
}

bool PasteurESPSys::lopy_subscribed()
{
    HTTPClient http;
    String espid = WiFi.macAddress();
    espid.replace(":", "");
    String route = SUBSCRIBED_ROUTE + espid;
    http.begin(route);
    int httpCode = http.GET();
    http.end();
    return httpCode == 200;
}

void PasteurESPSys::handle_message_changed()
{
    for (int i = 0; i < rest_server.args(); i++)
    {
        if (rest_server.argName(i) == "message")
        {
            conf.message = rest_server.arg(i);
            EEPROM_conf_write();
        }
    }
    rest_server.send(200, "text/plain", "Message changed.");
}

void PasteurESPSys::handle_menu()
{
    const String config_menu[3] = {
        "Scan wifi LoPy",
        "Afficher MAC",
        "Afficher message"};

    if (menu_state == MENU_MAC)
    {
        infoDisplay.set_text("MAC status", ((String)(WiFi.status() == WL_CONNECTED ? "Connecte" : "Deconnecte")), WiFi.macAddress());
        if (digitalRead(GPIO_SW3) == HIGH)
        {
            menu_state = MENU_CONFIG;
            delay(500);
        }
    }
    else if (menu_state == MENU_STATUS)
    {
        infoDisplay.set_text("Afficheur", "Message : ", conf.message);
        if (digitalRead(GPIO_SW3) == HIGH)
        {
            menu_state = MENU_CONFIG;
            delay(500);
        }
    }
    else if (menu_state == MENU_CONFIG)
    {
        int selected = infoDisplay.generate_menu("Menu", config_menu, 3);
        if (selected == 0)
        {
            system_state = SCANNING_WIFI;
        }
        else if (selected == 1)
            menu_state = MENU_MAC;
        else if (selected == 2)
            menu_state = MENU_STATUS;
    }
}

void PasteurESPSys::EEPROM_conf_write()
{
    const char *ssid = conf.wifi_ssid.c_str();
    const char *pass = conf.wifi_pass.c_str();
    const char *message = conf.message.c_str();

    for (int i = 0; i < 100; i++)
    {
        EEPROM.write(i, ssid[i]);
        EEPROM.write(i + 100, pass[i]);
        EEPROM.write(i + 200, message[i]);
    }
    EEPROM.commit();
}

void PasteurESPSys::EEPROM_conf_read()
{
    char ssid[100];
    char pass[100];
    char message[100];

    for (int i = 0; i < 100; i++)
    {
        ssid[i] = EEPROM.read(i);
        pass[i] = EEPROM.read(i + 100);
        message[i] = EEPROM.read(i + 200);
    }

    conf.wifi_ssid = ssid;
    conf.wifi_pass = pass;
    conf.message = message;
}

void PasteurESPSys::print_bootscreen()
{
    infoDisplay.set_text("Demarrage", "Hotel Pasteur", "Univ Rennes1");
    delay(2000);
}

void PasteurESPSys::run()
{
    switch (system_state)
    {
    case IDLE:
        handle_menu();
        break;
    case CONNECTING_WIFI:
        rest_server.close();
        if (connect_wifi())
        {
            system_state = SUBSCRIBING;
        }
        else
        {
            system_state = IDLE;
            menu_state = MENU_CONFIG;
        }
        break;
    case SCANNING_WIFI:
    {
        infoDisplay.set_text("Scan reseau", "Veuillez patienter...", "");
        int n = WiFi.scanNetworks();
        int n_valid = 0;
        String wifi[20];
        for (int i = 0; i < n; i++)
        {
            if (strstr(WiFi.SSID(i).c_str(), "Lopy") != NULL)
            {
                wifi[n_valid] = WiFi.SSID(i);
                n_valid++;
            }
        }
        wifi[n_valid] = "Annuler";
        n_valid++;
        int n_selected = infoDisplay.generate_menu("WIFI", wifi, n_valid);
        if (n_selected == n_valid - 1)
        {
            system_state = IDLE;
            menu_state = MENU_CONFIG;
        }
        else
        {
            conf.wifi_ssid = wifi[n_selected];
            conf.wifi_pass = WIFI_PASS;
            EEPROM_conf_write();
            system_state = CONNECTING_WIFI;
            menu_state = MENU_STATUS;
        }
    }
    break;
    case SUBSCRIBING:
        infoDisplay.set_text("Connexion", "Requetage du LoPy...", "");
        do
        {
            if (WiFi.status() != WL_CONNECTED)
            {
                infoDisplay.set_text("Erreur", "WIFI indisponible", "");
                delay(1500);
                system_state = CONNECTING_WIFI;
                return;
            }
            else
            {
                lopy_subscribe();
            }
        } while (!lopy_subscribed());
        rest_server.on("/cm", [&]() {
            handle_message_changed();
        });
        rest_server.begin();
        menu_state = MENU_STATUS;
        system_state = RUNNING;
        infoDisplay.set_text("Connexion", "Requetage du LoPy...", "termine.");
        delay(1500);
        break;
    case RUNNING:
        rest_server.handleClient();
        EEPROM_conf_read();
        matrixDisplay.set_text(conf.message);
        pasteurDisplay.set_text(conf.message);
        handle_menu();
        if (!lopy_subscribed())
        {
            system_state = SUBSCRIBING;
        }
        break;
    }
}