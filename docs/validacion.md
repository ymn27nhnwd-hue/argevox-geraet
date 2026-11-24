# Validación · 1.0.0 beta 2

Compilación nativa Windows, ARMv7 y AArch64. Validación SDK de identidad, ABI, bibliotecas, índices de parámetros, 5 páginas, imágenes y Q-Links sin errores ni advertencias.

Host simulator SDK

20 pruebas aprobadas, 0 advertencias, 0 fallos. Comprueba carga MPC, audio finito, bloques de 32 y 512 muestras, barrido de parámetros, estado de 93 parámetros, in-place, silencio y picos. Ejecución individual, media 2,78% del tiempo de bloque en PC, máximo 6,97%. Estos tiempos no son mediciones en MPC.

Pruebas DSP dirigidas

- Salida RMS 0 con ambas entradas silenciosas, solo portadora o solo micrófono, en modo Vocoder externo sin adiciones.
- Centroide armónico de salida 690 Hz ante análisis a 700 Hz y 2100 Hz ante análisis a 2140 Hz, con portadora de sierra de 110 Hz. La envolvente desplaza la energía del sonido según el estímulo.
- Centroide 454 Hz para el estímulo de 700 Hz con formantes y oscuridad graves.
- Ruido articulado, multifiltro, monitores de entrada, relleno de pausas y seis presets producen audio finito.
- Frecuencias de muestreo comprobadas 32, 44,1, 48 y 96 kHz.
- Un estado con los 49 identificadores de 0.2.1 restaura sus parámetros.

Los ensayos usan señales sintéticas. No certifican inteligibilidad de lenguaje real ni equivalencia sonora al VSM-201. Quedan pendientes escucha y CPU en MPC.

Empaquetado OpenMPC

Herramientas fijadas a 7199ebf12722f3828b4a4daac38e2cedf666cd0c. Se mantienen scripts en LF. Ambos ZIP se validan con catalog_check --catalog --expect-id argevox-geraet --expect-repo ymn27nhnwd-hue/argevox-geraet.
