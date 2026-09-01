# Proyecto-IoT
Trabajo en grupo laboratorio de Iot:

Enlace al proyecto en Wokwi:
https://wokwi.com/projects/472247770959126529

1. Calibración de dos puntos
- Pendiente (m): 1.0058f
- Desplazamiento (b): -0.2058f
- Tolerancia declarada: +-2.0 °C

Puntos de referencia:
- Punto 1 (Referencia R1): 6.0 °C
- Punto 2 (Referencia R2): 32.0 °C
- Punto 3 (Verificación R3): 21.0 °C

Resultado de verificación:
La lectura corregida en el tercer punto intermedio (21.0 °C) se encuentra dentro de la tolerancia de +-2.0 °C declarada para el proyecto.

2. Filtro y criterio de elección de N
- Filtro implementado: Media móvil con N = 5.
- Criterio de elección: Se eligió N = 5 porque reduce el ruido inyectado sobre la lectura del ADC manteniendo un retardo bajo, lo que permite una respuesta rápida y estable en la medición de temperatura.

  
--- GT2 ---

Nota Aclaratoria sobre la Simulación (GT1): Por instrucción directa de los profesores, la simulación de la GT1 se realizó y calibró utilizando un sensor de temperatura genérico simulado mediante un potenciómetro. Por este motivo, la calibración arrojó valores de ganancia (m = 1.0058f) y offset (b = -0.2058f) para grados Celsius, y se declaró una tolerancia de ±2.0 °C para el proyecto simulado. Dado que nuestro proyecto físico real utiliza dos sensores diferentes (MQ-135 y KY-038) que no fueron simulados en esa etapa, la verificación física a continuación detalla las mediciones reales de nuestros sensores, contrastadas con referencias propias de su magnitud física (ppm y dB) y extrapolando la tolerancia de ±2 a sus respectivas unidades.
Verificación física de los sensores (Apertura de la Sesión B)
Sensor de Calidad de Aire (MQ-135) Resultado físico (Línea base): Se ejecutó el procedimiento de caracterización en ambiente normal, obteniendo lecturas estables con una media de 447.2 ppm. Resultado de simulación de la GT1: No aplica directamente para contraste (la simulación GT1 arrojó una lectura corregida de 21.0 °C en el punto intermedio por instrucción docente de simular temperatura). Desviación observada: Se calculó la desviación absoluta entre el valor medido y el valor de referencia: |447.2 ppm - 450.0 ppm| = 2.8 ppm. Referencia empleada: Nivel teórico de concentración de CO2 para aire limpio en un espacio cerrado, establecido en 450 ppm. Tolerancia declarada antes de verificar: ±2 ppm (adaptada del criterio de la simulación).

Sensor de Sonido (KY-038) Resultado físico (Caracterización de ruido): Se ejecutó el procedimiento de detección de sonido ambiental, registrando fluctuaciones con una media de 57.93 dB. Resultado de simulación de la GT1: No aplica directamente para contraste (simulación orientada a temperatura por instrucción docente). Desviación observada: La media registrada (57.93 dB) supera el límite superior del rango de referencia (55 dB) por una desviación de +2.93 dB. Este valor es esperado y justificable, ya que la toma de datos incluyó ruidos adicionales puntuales (voces intencionales de prueba cerca del sensor) para comprobar su correcta respuesta dinámica. Referencia empleada: Nivel de ruido ambiental estándar esperado para un aula o sector cerrado (sin alteraciones acústicas mayores), el cual se tabula convencionalmente en un rango de 45 a 55 dB. Tolerancia declarada antes de verificar: ±2 dB (adaptada del criterio de la simulación).

Filtro y criterio de elección de N
Filtro implementado: Media móvil con N = 5. Criterio de elección: Se eligió N = 5 porque reduce el ruido inyectado sobre las lecturas de los sensores (suavizando las variaciones de los ppm del MQ-135 y los picos de decibeles del KY-038) manteniendo un retardo bajo, lo que permite una respuesta rápida y estable en la medición.







--- GT3 ---

Video de circuito en funcionamiento: 

https://drive.google.com/file/d/1pJf0zCyYcQ-HjyvcMk9zZ4_aJ8U5O6kv/view?usp=sharing


Código: 

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <esp_sleep.h>
#include <time.h> 

// ==========================================================
// 1. CONFIGURACIÓN RED, MQTT Y HORA (NTP)
// ==========================================================


const char* ssid = "Pichulaperro";
const char* password = "chaparritoUA";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* topico_alerta = "curso/e08/p2/alertas";
const char* topico_resumen = "curso/e08/p2/nodo1";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -14400;    // Zona Horaria de Chile (ajusta si hay cambio de hora)
const int   daylightOffset_sec = 3600; 


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
// 3. PANTALLA OLED
// ==========================================================



#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



// ==========================================================
// 4. CONSTANTES Y FILTROS FÍSICOS (MQ-135)
// ==========================================================




const float V_REF = 3.3; 
const float CUENTAS_MAX = 4095.0;
const float ESCALA_SENSOR = 484.84;  
const float OFFSET_SENSOR = 400.0;   
const float M_CAL = 1.0; 
const float B_CAL = 0.0;

const uint8_t MAX_MUESTRAS = 15; 
float ventana[MAX_MUESTRAS];
uint8_t muestras_validas = 0;
uint8_t indice_ventana = 0;

float cuentas_a_fisica(int cuentas) {
  return ((V_REF * cuentas) / CUENTAS_MAX) * ESCALA_SENSOR + OFFSET_SENSOR;
}

float aplicar_calibracion(float valor_medido) {
  return M_CAL * valor_medido + B_CAL;
}

float mediana() {
  if (muestras_validas == 0) return OFFSET_SENSOR; 
  float copia[MAX_MUESTRAS]; 
  for (uint8_t i = 0; i < muestras_validas; i++) copia[i] = ventana[i];
  for (uint8_t i = 1; i < muestras_validas; i++) {     
    float clave = copia[i];
    int8_t j = i - 1;
    while (j >= 0 && copia[j] > clave) { copia[j + 1] = copia[j]; j--; }
    copia[j + 1] = clave;
  }
  return copia[muestras_validas / 2];
}



// ==========================================================
// 5. MÁQUINA DE ESTADOS Y TIEMPOS (FSM)
// ==========================================================



enum EstadoSistema : uint8_t { CALENTANDO, MONITOREANDO, ALERTA_RUIDO, ALERTA_CO2, ERROR_SEGURO };
EstadoSistema estadoActual = CALENTANDO;

// TIEMPOS OPTIMIZADOS PARA BATERÍA 7.4V (3.5 DÍAS DE AUTONOMÍA)
const uint32_t TIEMPO_CALENTAMIENTO_MS = 240000; // 4 minutos de calentamiento
const uint32_t TIEMPO_ACTIVO_MS = 180000;        // 3 minutos activo
const uint64_t TIEMPO_DORMIR_US = 3180000000ULL; // 53 minutos en microsegundos (Total ciclo = 60m)

uint32_t tiempoInicioActividad = 0;
uint32_t tiempoAnteriorMuestreo = 0;
uint32_t tiempoAnteriorPantalla = 0;
uint32_t t_entrada = 0;

bool mostrarRuidoPantalla = true;
int ruidoActual = 0;
float aireActual = 0.0;

const int MAX_MUESTRAS_RUIDO = 500; 
int arregloRuido[MAX_MUESTRAS_RUIDO];
int contadorMuestras = 0;
int ruidoMediana = 0;

float co2_maximo = 0.0;
float co2_minimo = 9999.0;
int ruido_maximo = 0;
int ruido_minimo = 9999;

uint8_t lecturasInvalidas = 0; 

RTC_DATA_ATTR bool alertaRuidoEnviada = false;
RTC_DATA_ATTR bool alertaCO2Enviada = false;
bool despertarPorBoton = false; 

WiFiClient espClient;
PubSubClient client(espClient);

void cambiar(EstadoSistema e) {
  estadoActual = e;
  t_entrada = millis();
  Serial.printf("[%lu ms] -> estado %u\n", millis(), (unsigned)e);
}

bool leerBotonFuerzaEnvio() {
  static uint32_t t_ultimo_boton = 0;
  if (digitalRead(pinBoton) == HIGH && (millis() - t_ultimo_boton >= 50)) {
    t_ultimo_boton = millis();
    return true;
  }
  return false;
}

void reconectarMQTT() {
  if (!client.connected()) {
    if (client.connect("ESP32_IoT_Node_grupo1")) {
      Serial.println("Conectado al Broker MQTT");
    }
  }
}



// ==========================================================
// 6. SETUP 
// ==========================================================



void setup() {
  Serial.begin(115200);
  
  pinMode(pinAire, INPUT);
  pinMode(pinRuidoAnalog, INPUT);
  pinMode(pinBoton, INPUT_PULLDOWN);
  pinMode(pinLed, OUTPUT);
  pinMode(pinRele, OUTPUT);
  
  digitalWrite(pinRele, RELE_ENCENDIDO); 

  esp_sleep_wakeup_cause_t motivo = esp_sleep_get_wakeup_cause();
  if (motivo == ESP_SLEEP_WAKEUP_EXT1) {
    despertarPorBoton = true;
  }

  if(display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    display.clearDisplay();
    display.display();
  }

  WiFi.mode(WIFI_OFF); 
  tiempoInicioActividad = millis();
}



// ==========================================================
// 7. LOOP PRINCIPAL NO BLOQUEANTE
// ==========================================================



void loop() {
  uint32_t ahora = millis();

  // --------------------------------------------------------
  // FASE 1: CALENTAMIENTO (4 MINUTOS)
  // --------------------------------------------------------
  if (estadoActual == CALENTANDO) {
    if (ahora - tiempoAnteriorPantalla >= 1000) {
      tiempoAnteriorPantalla = ahora;
      int segundosRestantes = (TIEMPO_CALENTAMIENTO_MS - (ahora - tiempoInicioActividad)) / 1000;
      
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 10);
      display.println(F("Calentando MQ-135..."));
      display.println(F("WiFi APAGADO"));
      display.setTextSize(3);
      display.setCursor(30, 35);
      display.print(segundosRestantes);
      display.display();
    }

    if (ahora - tiempoInicioActividad >= TIEMPO_CALENTAMIENTO_MS) {
      display.clearDisplay(); display.setTextSize(1); display.setCursor(0, 20);
      display.println(F("Conectando WiFi...")); display.display();

      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) { delay(500); }
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); 
      client.setServer(mqtt_server, mqtt_port);

      cambiar(MONITOREANDO);
      tiempoInicioActividad = millis(); 
    }
  } 


  
  // --------------------------------------------------------
  // FASE 2: MUESTREO Y ALERTAS (3 MINUTOS)
  // --------------------------------------------------------


  
  else {
    reconectarMQTT();
    client.loop();
    
    // A. MUESTREO FÍSICO
    if (ahora - tiempoAnteriorMuestreo >= 500) {
      tiempoAnteriorMuestreo = ahora;

      // Peak-to-Peak (50ms)
      int max_val = 0;
      int min_val = 4095;
      uint32_t inicio_audio = millis();
      while(millis() - inicio_audio < 50) {
        int val = analogRead(pinRuidoAnalog);
        if(val > max_val) max_val = val;
        if(val < min_val) min_val = val;
      }
      int amplitudSonido = max_val - min_val; 
      
      int adcAire = analogRead(pinAire);

      // Failsafe (Seguridad de cable suelto)
      if (adcAire <= 10) lecturasInvalidas++;
      else lecturasInvalidas = 0;

      if (lecturasInvalidas >= 3 && estadoActual != ERROR_SEGURO) {
        cambiar(ERROR_SEGURO);
      }

      if (estadoActual != ERROR_SEGURO) {
        ruidoActual = map(amplitudSonido, 0, 4095, 30, 120);

        if (contadorMuestras < MAX_MUESTRAS_RUIDO) {
          arregloRuido[contadorMuestras] = ruidoActual;
          contadorMuestras++;
        }

        if (ruidoActual > ruido_maximo) ruido_maximo = ruidoActual;
        if (ruidoActual < ruido_minimo) ruido_minimo = ruidoActual;

        float aireCrudo = aplicar_calibracion(cuentas_a_fisica(adcAire));
        ventana[indice_ventana] = aireCrudo;
        indice_ventana = (indice_ventana + 1) % MAX_MUESTRAS;
        if (muestras_validas < MAX_MUESTRAS) muestras_validas++;

        aireActual = mediana(); 

        if (aireActual > co2_maximo) co2_maximo = aireActual;
        if (aireActual < co2_minimo) co2_minimo = aireActual;

        // Histéresis y Alertas
        if (ruidoActual >= 50) { 
          if (!alertaRuidoEnviada) {
            String payloadRuido = "{\"alerta\":\"Ruido\",\"valor\":" + String(ruidoActual) + "}";
            client.publish(topico_alerta, payloadRuido.c_str());
            alertaRuidoEnviada = true; 
          }
          if (estadoActual != ALERTA_RUIDO) cambiar(ALERTA_RUIDO);
        } else if (ruidoActual <= 75) {
          alertaRuidoEnviada = false; 
        }

        if (aireActual >= 700.0) {
          if (!alertaCO2Enviada) {
            client.publish(topico_alerta, "{\"alerta\":\"Aire\",\"valor\":""}"); 
            alertaCO2Enviada = true; 
          }
          if (estadoActual == MONITOREANDO) cambiar(ALERTA_CO2);
        } else if (aireActual <= 600.0) {
          alertaCO2Enviada = false; 
        }

        if (ruidoActual <= 75 && aireActual <= 600.0 && estadoActual != MONITOREANDO) {
          cambiar(MONITOREANDO);
        }
      }
    }

    // B. PANTALLA OLED
    if (ahora - tiempoAnteriorPantalla >= 2000 && estadoActual != ERROR_SEGURO) {
      tiempoAnteriorPantalla = ahora;
      mostrarRuidoPantalla = !mostrarRuidoPantalla;
      
      display.clearDisplay(); display.setTextSize(2); display.setCursor(0, 10);
      
      if (mostrarRuidoPantalla) {
        display.println(F("RUIDO:"));
        display.print(ruidoActual); display.println(F(" dB"));
      } else {
        display.println(F("AIRE PPM:"));
        display.println(aireActual, 1);
      }
      display.display();
    }

    // C. MÁQUINA DE ESTADOS
    switch (estadoActual) {
      case MONITOREANDO:
        digitalWrite(pinLed, LOW);
        break;
      case ALERTA_RUIDO:
      case ALERTA_CO2:
        digitalWrite(pinLed, HIGH);
        break;
      case ERROR_SEGURO:
        digitalWrite(pinRele, RELE_APAGADO); 
        digitalWrite(pinLed, HIGH); 
        display.clearDisplay(); display.setTextSize(2); display.setCursor(0, 20);
        display.println(F("ERROR!")); display.setTextSize(1); display.println(F("Cable Suelto"));
        display.display();
        break;
      case CALENTANDO:
        break; 
    }
  }




  // --------------------------------------------------------
  // D. CIERRE DE CICLO Y DEEP SLEEP (NTP)
  // --------------------------------------------------------



  
  bool terminoCiclo = (estadoActual != CALENTANDO) && (ahora - tiempoInicioActividad >= TIEMPO_ACTIVO_MS);

  if (terminoCiclo || leerBotonFuerzaEnvio() || despertarPorBoton) {
    despertarPorBoton = false; 
    
    // Si despertó por botón físico estando sin WiFi, forzamos conexión
    if (WiFi.status() != WL_CONNECTED) {
      display.clearDisplay(); display.setCursor(0, 10); display.setTextSize(1);
      display.print(F("Conectando...")); display.display();
      WiFi.mode(WIFI_STA); WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) { delay(100); }
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      client.setServer(mqtt_server, mqtt_port); reconectarMQTT();
    }

    // Cálculo Mediana Ruido
    if (contadorMuestras > 0) {
      for (int i = 0; i < contadorMuestras - 1; i++) {
        for (int j = 0; j < contadorMuestras - i - 1; j++) {
          if (arregloRuido[j] > arregloRuido[j + 1]) {
            int temporal = arregloRuido[j];
            arregloRuido[j] = arregloRuido[j + 1];
            arregloRuido[j + 1] = temporal;
          }
        }
      }
      int mitad = contadorMuestras / 2;
      if (contadorMuestras % 2 == 0) { ruidoMediana = (arregloRuido[mitad - 1] + arregloRuido[mitad]) / 2; } 
      else { ruidoMediana = arregloRuido[mitad]; }
    } else { ruidoMediana = 0; }
    contadorMuestras = 0; 
    
    // Armar Resumen JSON Final
    String payload = "{\"mediana_co2\":" + String(aireActual) + 
                     ",\"max_co2\":" + String(co2_maximo) + 
                     ",\"min_co2\":" + String(co2_minimo) + 
                     ",\"mediana_ruido\":" + String(ruidoMediana) + 
                     ",\"max_ruido\":" + String(ruido_maximo) + 
                     ",\"min_ruido\":" + String(ruido_minimo) + "}";
                     
    client.publish(topico_resumen, payload.c_str());
    delay(500); 

    // Apagar energía del sensor y LED
    digitalWrite(pinRele, RELE_APAGADO); 
    digitalWrite(pinLed, LOW);
    
    // Cálculo Hora Despertar
    struct tm timeinfo;
    String textoDespertar = "--:--"; 
    if (getLocalTime(&timeinfo)) {
      time_t ahora_epoch;
      time(&ahora_epoch); 
      ahora_epoch += (53 * 60); // Suma los 53 minutos de sueño a la hora actual
      struct tm *horaFutura = localtime(&ahora_epoch); 
      char buffer[6]; 
      sprintf(buffer, "%02d:%02d", horaFutura->tm_hour, horaFutura->tm_min);
      textoDespertar = String(buffer);
    }

    // Mostrar mensaje final
    display.clearDisplay(); display.setTextSize(1); display.setCursor(0, 10);
    display.println(F("Datos Enviados!")); display.println(F("Despierta a las:"));
    display.setTextSize(3); display.setCursor(20, 35); display.print(textoDespertar);
    display.display();
    
    delay(4000); 
    display.ssd1306_command(SSD1306_DISPLAYOFF); // APAGADO FÍSICO DE LA PANTALLA

    // Desconectar servicios
    client.disconnect();
    WiFi.disconnect(true);

    // Configurar y ejecutar Deep Sleep
    esp_sleep_enable_ext1_wakeup(1ULL << pinBoton, ESP_EXT1_WAKEUP_ANY_HIGH);
    esp_sleep_enable_timer_wakeup(TIEMPO_DORMIR_US);
    esp_deep_sleep_start();
  }
}
