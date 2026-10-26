#pragma once
#include <ArduinoJson.h>

// ==========================================================
// 1. CONFIGURACIÓN RED Y MQTT (Red de la Universidad)
// ==========================================================
// ¡ATENCIÓN!: Rellenar con los datos del laboratorio
const char* ssid = ""; 
const char* password = ""; 
const char* mqtt_server = ""; // Ya no es HiveMQ, usa la IP local
const int mqtt_port = ;

// --- CREDENCIALES PROPIAS DEL EQUIPO ---
// La ACL del servidor limita tu acceso a tu propio prefijo
const char* mqtt_user = ""; // Tu usuario MQTT asignado
const char* mqtt_pass = ""; // Tu clave MQTT asignada

const char* mqtt_client_id = ""; 

// --- TÓPICOS ESTRICTOS DE LA ASIGNATURA ---
// Regla clave: Las mayúsculas importan (E08 ≠ e08).
// La telemetría principal debe usar exactamente 4 niveles.
const char* topico_resumen = "curso/E08/P02/nodo1"; 

// Los tópicos auxiliares deben colgar del nivel del nodo y usar los nombres oficiales.
const char* topico_estado  = "curso/E08/P02/nodo1/estado"; // Corregido de "status" a "estado"
const char* topico_cmd     = "curso/E08/P02/nodo1/cmd"; 
const char* topico_alerta  = "curso/E08/P02/nodo1/alerta"; // Movido al prefijo del equipo

// --- CONFIGURACIÓN NTP Y TIMEOUT ---
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -14400;
const int   daylightOffset_sec = 3600;
const uint32_t WIFI_TIMEOUT_MS = 15000;

// ==========================================================
// 2. CONFIGURACIÓN DE PINES
// ==========================================================
const int pinAire = 34;
const int pinRuidoAnalog = 32;
const int pinBoton = 25;
const int pinLed = 33;
const int pinRele = 4;

#define RELE_ENCENDIDO HIGH
#define RELE_APAGADO LOW

// ==========================================================
// 3. CONSTANTES Y FILTROS FÍSICOS (MQ-135)
// ==========================================================
const float V_REF = 3.3;
const float CUENTAS_MAX = 4095.0;
const float ESCALA_SENSOR = 484.84;
const float OFFSET_SENSOR = 400.0;
const float M_CAL = 1.0;
const float B_CAL = 0.0;
