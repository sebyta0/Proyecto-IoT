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
