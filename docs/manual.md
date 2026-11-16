# Argevox Gerät
## Manual de usuario · Versión 0.2.1

Argevox Gerät es un vocoder de 20 bandas para MPC standalone. El sinte aporta la nota o el acorde y el micrófono aporta la articulación. El efecto recibe ambos por un submix estéreo y devuelve la señal procesada al centro.

Inspirado en el **Sennheiser VSM-201**, el modelo que Kraftwerk usó en *The Man-Machine* (1978) y *Computer World* (1981). [Referencia histórica y fotos](https://www.juliensauctions.com/en/items/2088294/florian-schneider-a-kling-klang-sennheiser-vsm-201-vocoder-in-original-case).

## 1. Instalar o actualizar

Necesitás una MPC con **MPC OS 3.10.1 o posterior**. La **One+** se probó en hardware con versiones anteriores del plugin. La versión 0.2.1 pasó las pruebas del simulador y la validación del SDK.

1. Descomprimí el ZIP instalable. Copiá la carpeta **Argerax - VST - Argevox Gerät** a **VST2 Plugins**, en la raíz de una SD o pendrive. Adentro de la carpeta del plugin deberían verse `plugin.json`, `version.xml`, `VST` y `Plugin Skins`.
2. Si actualizás, guardá la versión anterior fuera de **VST2 Plugins**. Las versiones **Argevox Geräte** y **Argevox Gerät** usan el mismo identificador.
3. En **Preferences → Enhancements (Beta)** activá **Enable Enhancements** y **External Plugin Support**. Al actualizar, reiniciá la MPC con la unidad conectada.
4. Abrí un slot de insert en el submix y buscá **External VST Plugins → Argerax → Argevox Gerät**.

## 2. Preparar el proyecto

| Señal | Pista en la MPC | Panorama | Salida |
|---|---|---|---|
| Portadora | Keygroup con **Digital Saw** o pista de **Mini D** | Todo a la izquierda | Sub 1 |
| Voz en vivo | Audio, **Input 1**, **Monitor → In** | Todo a la derecha | Sub 1 |
| Efecto | **Argevox Gerät** en un slot de insert | Sub 1 centrado | Salida principal |

- Conectá la **salida de línea** del preamp de micrófono al **Audio In 1**. Ajustá su ganancia para que tu voz habitual llegue con margen antes de recortar.
- Para la portadora nativa, abrí **Track Edit → Samples/Oscs** en el Keygroup. En la primera capa elegí **OSC → Digital Saw**. Un filtro abierto y una nota sostenida facilitan la primera prueba.
- Si usás un teclado dedicado, elegí su puerto y canal MIDI en la pista del sinte. Usá **Monitor → In** en esa pista si querés seguir tocándola al seleccionar otra.
- Cerrá los envíos de las dos pistas al probar. Si querés reverb y delay en el resultado, usá **Send 1** y **Send 2** desde **Sub 1**.

**Primera prueba.** Sostené una nota y hablá. **OSZ / L** indica el sinte y **MIK / R** la voz. Repetí con un acorde.

## 3. Controles en pantalla

| Pestaña | Control | Función |
|---|---|---|
| VOCODER | **Stimme** | Ganancia de la voz antes del análisis de bandas. |
| VOCODER | **Träger** | Ganancia de la portadora. |
| VOCODER | **Konsonanten** | Cantidad de ruido filtrado que ayuda a reproducir consonantes. |
| VOCODER | **Reaktion** | Respuesta de las bandas. Valores bajos siguen la articulación más rápido. Valores altos suavizan las transiciones. |
| STIMME | **Schwelle** | Umbral de apertura de −80 a 0 dBFS. Mide la entrada digital MIK / R antes de Stimme. En −80 dBFS se desactiva el umbral. |
| STIMME | **Entzischung** | Reducción dinámica de sibilantes hasta 6 dB. En 0 dB se desactiva. El ajuste inicial es 3 dB. |
| BAND 01 / BAND 11 | **Band 1–20** | Faders táctiles con ±18 dB de ajuste por banda de síntesis. El valor en Hz o kHz indica su frecuencia central. |

Los indicadores de **Sprachanalyse** muestran la actividad de voz de graves a agudos. Los presets son **Klassisch**, **Roboter** y **Weich**.

### Ajustar Schwelle

Dejá el micrófono en **Monitor In**, sostené un acorde y hacé silencio. Partí de **−48 dBFS**. Si el ruido de fondo abre el efecto, subí Schwelle hasta que se calle. Hablá a tu volumen habitual y bajá el umbral si se pierden comienzos de palabras. **dBFS** mide la señal digital en el submix. La ganancia analógica del preamp se ajusta por separado.

## 4. Si algo falla

- **El insert no aparece.** Confirmá la versión de MPC OS, las dos opciones de Enhancements y que la carpeta del plugin esté directamente dentro de **VST2 Plugins**. Buscalo en el selector de inserts de la MPC.
- **Hay señal en un solo indicador.** Revisá las salidas de ambas pistas, su panorama y **Monitor In** en la pista de audio.
- **El sinte se oye sin hablar.** Con auriculares y parlantes apagados, desactivá el insert momentáneamente. Con el monitoreo del micrófono apagado, el acorde debería oírse solo por L. Después soltá el acorde, volvé a **Monitor In** y confirmá que la voz se oiga solo por R. Reactivá el insert y ajustá Schwelle. Si mutear Sub 1 deja el sinte sonando, revisá los envíos y las rutas directas.
- **La voz quedó opaca.** Bajá un poco **Entzischung**, probá **Reaktion** más rápida o recuperá una banda alta de a una. Compará con los faders centrados en 0 dB.

[Guía de instalación del SDK oficial](https://github.com/inmusic-software/mpc-plugin-builder-sdk/blob/main/docs/install-and-test.md)

