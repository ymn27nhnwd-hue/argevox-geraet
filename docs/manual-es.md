# Argevox Gerät — 1.0.1

[English](manual-en.md) · [Deutsch](manual-de.md)

Fabricante y desarrollo Argerax. Versión estable 1.0.1.


Instalación manual: descargá **Argevox-Geraet-1.0.1-mpc.zip**, descomprimilo y copiá la carpeta completa **Argerax - VST - Argevox Gerät** dentro de **VST2 Plugins**. Si descargás un ZIP de OpenMPC por arquitectura, esa carpeta está dentro de **portable**. No ejecutes los archivos `.sh` en Windows ni macOS. La MPC One+ usa **armv7**. La instalación desde OpenMPC y el hardware AArch64 siguen sin verificar para 1.0.1.
## Corrección de empaquetado 1.0.1

El procesamiento de audio y la distribución de parámetros se conservan de 1.0.0 beta 2. Esta revisión actualiza las plantillas del instalador de OpenMPC y los identificadores de versión. La prueba de hardware existente se registra como 1.0.0, que coincide con la versión numérica del manifiesto de ese motor. El 25 de noviembre de 2026 el autor confirmó la instalación manual y el funcionamiento de 1.0.1 beta 1 en MPC One+ con MPC OS 3.10.1. La estable conserva los mismos binarios ARM; solo cambia la documentación y el encabezado gráfico.

El autor midió aproximadamente 34% de CPU en el proyecto completo con Mini D y Argevox activos, frente a 20% con Argevox en bypass. La diferencia aproximada es de 14 puntos porcentuales en ese proyecto. No es una medición aislada del consumo del plugin.

## Instalación y primer arranque

Requiere MPC OS 3.10.1 o posterior y Preferences → Enhancements (Beta) → Enable Enhancements y External Plugin Support activados. Copiá la carpeta `Argerax - VST - Argevox Gerät` del ZIP manual a `VST2 Plugins` en la raíz de tu unidad. Reiniciá la MPC o volvé a explorar los plugins según el flujo del dispositivo.

Esta versión usa la misma identidad de plugin que 0.2.1 y ocupa su lugar. Guardá una copia del instalador 0.2.1 y de los proyectos antes de probarla. Conserva los identificadores y el orden de los primeros 49 parámetros, aunque el motor cambia el sonido. Los parámetros nuevos aparecen al final. Los proyectos antiguos conservan sus ganancias y umbral, que pueden necesitar reajuste para este motor.

## Ruteo

Sinte u oscilador de la MPC completamente a L, micrófono completamente a R, ambos al mismo submix estéreo. Argevox como insert del submix. Monitor del micrófono en In. La salida procesada es mono centrada y se escribe en ambos canales. La entrada L es la portadora y R es el modulador.

Primera comparación recomendada

1. Cargá el preset **Klar 201**. Stimme y Träger en 0 dB, bandas en 0, Entzischung en 0. Mantené una nota de sierra grave en Mini D con el filtro abierto.
2. Para aislar el umbral, poné **Schwelle en −80 dBFS** durante la prueba. Después subilo hasta silenciar el ruido de fondo sin perder sílabas.
3. Hablá normalmente. **Stimme AGC** y **Träger AGC** regulan el nivel interno con ganancia acotada. Ausgang ajusta la escucha.
4. Compará con **Dunkle Stimme**. **Formanten** desplaza la envolvente espectral sin cambiar las notas del sintetizador. Los valores negativos dan resonancias más graves. **Dunkelheit** atenúa progresivamente las bandas superiores.
5. Si faltan consonantes, aumentá Konsonanten gradualmente. **Klarheit** agrega una fracción de la voz real filtrada en agudos durante sonidos sordos. Su valor inicial es 0 para evaluar primero la síntesis.

## Controles

| Página | Controles y función |
| --- | --- |
| STIMME | Stimme, Träger, Konsonanten, Reaktion; Schwelle, Entzischung, ambos AGC; Betriebsart, Stimmart, Tonquelle y Tonhöhe. Tres filas de cuatro controles. |
| KLANG | Ausgang, Formanten, Dunkelheit y Klarheit; Kontur, Rosa Rauschen, Sprachanteil y Pausenpegel. |
| BÄNDER | Las 20 ganancias táctiles de síntesis en dos filas de diez, con frecuencia central y medidor de actividad por banda. |
| SPRACHE | Las 20 mezclas táctiles de voz filtrada en dos filas de diez. |
| PAUSEN | Pausenmodus, Pausenpegel, Schwelle y Ausgang; los diez controles Pausenpaar debajo. |

Cada página tiene al menos cuatro Q-Link asignados. Los grupos se ordenan por filas. En BÄNDER y SPRACHE hay cuatro bancos de cuatro asignaciones para las bandas 1–8 y 11–18. Las bandas 9, 10, 19 y 20 se ajustan por pantalla táctil. Los 20 controles permanecen visibles a la vez. Los nombres de parámetros y los valores guardados son los de beta 1.

### Betriebsart

- **Vocoder** aplica las envolventes al sonido de excitación y suma las adiciones seleccionadas.
- **Multifilter** procesa la voz mediante los filtros y los 20 controles Sprache. No requiere portadora.
- **Stimme** permite escuchar el micrófono acondicionado y comprobar sus niveles.
- **Träger** permite escuchar la portadora acondicionada.

Los modos de escucha Stimme y Träger pasan a salida la entrada seleccionada. La protección de picos y Ausgang siguen activos.

### Stimmart y fuentes

- **Automatik** mezcla excitación tonal y ruido en función del balance espectral de la voz. Konsonanten controla el aporte automático de ruido.
- **Tonhaft** conserva la excitación tonal.
- **Rauschen** emplea ruido para todas las bandas y permite susurros articulados.
- **Tonquelle** selecciona Extern L, Säge intern o Puls intern. El oscilador interno es de prueba, con frecuencia fija regulada por Tonhöhe. Este insert no recibe notas MIDI.
- **Rosa Rauschen** cambia el ruido blanco por una aproximación digital de ruido rosa.

### Formantes, oscuridad y articulación

Formanten desplaza la envolvente entre bandas de −12 a +12 semitonos. Dunkelheit atenúa progresivamente las bandas superiores. Kontur reduce las bandas de análisis débiles respecto a las dominantes. Reaktion controla los tiempos de ataque y liberación; sus valores bajos siguen cambios más rápidos. Entzischung reduce el exceso de energía aguda en sonidos sordos. Klarheit añade voz real filtrada y puede dejarse en 0 para evaluar la voz totalmente sintetizada.

### Pausas

**Pausenmodus** ofrece Aus, Global y Paarweise. Global aplica Pausenpegel a todo el banco. Paarweise también usa los diez controles Pausenpaar. El relleno es intencionalmente audible cuando el detector cierra por ausencia de voz. Requiere una portadora activa. Con Schwelle en −80 el detector permanece abierto y el relleno no entra. Es una función creativa que debe estar en Aus para la prueba de silencio normal.

### Presets

- **Klar 201** — articulación de referencia con excitación externa y oscuridad 0.
- **Dunkle Stimme** — formantes −4 semitonos y oscuridad 45%, excitación externa.
- **Tiefe Maschine** — pulso interno a 85 Hz, formantes −7 y oscuridad 65%.
- **Flüstern** — ruido rosa articulado, inicialmente con fuente externa. Seleccioná una fuente interna si querés probarlo sin sinte.
- **Multifilter** — voz filtrada con ajustes táctiles por banda.
- **Pausenbrücke** — demostración de relleno global con portadora externa sostenida.

## Solución de problemas

Si Vocoder queda en silencio, comprobá ambos medidores de entrada y el ruteo L/R. Poné Schwelle temporalmente en −80 y seleccioná Extern L para usar Mini D. Si solo escuchás una nota sostenida, confirmá que el micrófono vaya a R y que Pausenmodus esté en Aus. Si la voz suena opaca, empezá con Klar 201, Entzischung 0, Dunkelheit 0 y las bandas en 0 dB. Guardá la configuración que funciona como preset o proyecto de MPC.

## Referencia VSM-201 y alcance de esta versión

Se implementó una interpretación funcional digital de los bloques del manual, con banco de 20 bandas, envolventes, AGC, excitación tonal/ruido, selección automática, mezcla de voz filtrada, multifiltro, monitores de entrada y relleno global o por pares.

Las frecuencias centrales son lecturas aproximadas de la Ilustración 8 del fabricante, redondeadas en Hz

140, 230, 330, 450, 580, 700, 850, 1000, 1170, 1360, 1570, 1830, 2140, 2500, 2900, 3400, 4000, 4800, 5800, 7500.

Cada canal de análisis y síntesis usa tres resonadores digitales en cascada. Las envolventes emplean rectificación y dos etapas de suavizado con tiempos dependientes de la banda. La modulación precede a los filtros de síntesis. La detección sonoro/sordo, el AGC, las constantes de tiempo, el ruido rosa y los valores Q son diseños digitales propios aproximados. Requieren comparación auditiva y mediciones adicionales para acercarse al equipo original. No se ha realizado una emulación por componentes ni una comparación calibrada con un VSM-201 físico.

La adaptación estéreo tiene dos entradas y dos salidas. Los conectores CV, las 20 salidas físicas de envolvente, las entradas independientes de excitación sorda y las conexiones del VSM-201 a equipos externos quedan fuera de esta interfaz. Las envolventes tienen medidores en pantalla. Para el desplazamiento de formantes se remapea la envolvente entre bandas. La oscuridad y Klarheit son extensiones de Argerax.

## Validación y prueba en hardware

Los ensayos automáticos verifican carga, estado de 93 parámetros, seis presets, bloques de 32 a 512 muestras, señal finita, procesamiento in-place, silencio, migración de los 49 parámetros publicados, funciones de escucha y multifiltro, ruido articulado, relleno de pausas y movimiento del espectro al desplazar formantes.

El autor confirmó el funcionamiento de beta 1 en MPC One+ con MPC OS 3.10.1, con acordes y voces oscuras, y destacó el preset Klar 201. Beta 2 utiliza los mismos binarios DSP y reorganiza la interfaz. La comparación de CPU del proyecto está registrada arriba. La instalación de esta corrección mediante OpenMPC sigue pendiente de confirmación. La prueba del autor no constituye una comparación calibrada con un VSM-201 físico.

## Fuentes

- Manual original de Sennheiser, especialmente apartados 2.4–2.11, 3.5–3.7 y datos técnicos. https://rubidium.se/~magnus/synths/companies/sennheiser/
- Curva original de filtros, Ilustración 8. https://rubidium.se/~magnus/synths/companies/sennheiser/Sennheiser_VSM201_manual_il8_200.jpg
- Esquema original B101–B110 de los canales espectrales. https://rubidium.se/~magnus/synths/companies/sennheiser/Sennheiser_VSM201_schematic_b101_b110_200.jpg

Copyright © 2026 Argerax por los aportes originales. MIT. Los componentes del MPC Plugin Builder SDK reutilizados conservan los avisos de inMusic. Consultá LICENSE.
