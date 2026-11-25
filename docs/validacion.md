# Validación · 1.0.1 estable

Compilación nativa Windows, ARMv7 y AArch64. Validación SDK de identidad, ABI, bibliotecas, índices de parámetros, 5 páginas, imágenes y Q-Links sin errores ni advertencias.

Host simulator SDK

20 pruebas aprobadas, 0 advertencias, 0 fallos. Comprueba carga MPC, audio finito, bloques de 32 y 512 muestras, barrido de parámetros, estado de 93 parámetros, in-place, silencio y picos. Ejecución individual, media 2,50% del tiempo de bloque en PC, máximo 6,16%. Estos tiempos no son mediciones en MPC.

Pruebas DSP dirigidas

- Salida RMS 0 con ambas entradas silenciosas, solo portadora o solo micrófono, en modo Vocoder externo sin adiciones.
- Centroide armónico de salida 690 Hz ante análisis a 700 Hz y 2100 Hz ante análisis a 2140 Hz, con portadora de sierra de 110 Hz. La envolvente desplaza la energía del sonido según el estímulo.
- Centroide 454 Hz para el estímulo de 700 Hz con formantes y oscuridad graves.
- Ruido articulado, multifiltro, monitores de entrada, relleno de pausas y seis presets producen audio finito.
- Frecuencias de muestreo comprobadas 32, 44,1, 48 y 96 kHz.
- Un estado con los 49 identificadores de 0.2.1 restaura sus parámetros.

Los ensayos usan señales sintéticas. No certifican inteligibilidad de lenguaje real ni equivalencia sonora al VSM-201. El autor aprobó la escucha del motor en MPC One+ y comunicó 34% de CPU para el proyecto completo activo frente a 20% en bypass. Son aproximadamente 14 puntos porcentuales, no un benchmark aislado del plugin. El autor confirmó la instalación manual de 1.0.1 beta 1 en MPC One+ con OS 3.10.1 el 25 de noviembre de 2026. La estable conserva esos mismos binarios ARM; solo actualiza documentación y el encabezado gráfico. La instalación mediante OpenMPC y el hardware AArch64 siguen sin verificar.

Empaquetado OpenMPC

Herramientas fijadas a 93804291aac2623a62cb2baacf8125c7270fe4e7. Se mantienen scripts en LF. Ambos ZIP se validan con catalog_check --catalog --expect-id argevox-geraet --expect-repo ymn27nhnwd-hue/argevox-geraet.
