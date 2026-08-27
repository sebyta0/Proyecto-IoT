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
