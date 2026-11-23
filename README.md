# Argevox Gerät

Vocoder de 20 bandas para MPC standalone, desarrollado por **Argerax**. Usá el micrófono en vivo como modulador y un oscilador nativo de la MPC como portadora. **Mini D** también se probó como portadora en la MPC One+. Inspirado en el **Sennheiser VSM-201**, el modelo que Kraftwerk usó en *The Man-Machine* (1978) y *Computer World* (1981). [Referencia histórica y fotos](https://www.juliensauctions.com/en/items/2088294/florian-schneider-a-kling-klang-sennheiser-vsm-201-vocoder-in-original-case).

![Pantalla principal de Argevox Gerät](docs/interfaz.png)

## Estado de las pruebas

El autor confirmó la instalación de la versión 0.2.1 desde el repositorio de OpenMPC mediante el Plugin Manager en su MPC One+, con MPC OS 3.10.1. El plugin carga y produce audio usando Mini D como portadora y micrófono como modulador. La prueba se registró el 23 de noviembre de 2026.

Las pruebas musicales detectaron dificultades para ajustar los niveles y una inteligibilidad de voz insuficiente. Con Konsonanten y las ganancias de las bandas en 0 la señal resultó más equilibrada, aunque la articulación necesita mejoras.

## Instalación rápida

1. Descomprimí el ZIP instalable de la versión 0.2.1. Copiá **Argerax - VST - Argevox Gerät** a **VST2 Plugins** en la raíz de una SD o pendrive.
2. En **MPC OS 3.10.1 o posterior**, activá **Enable Enhancements** y **External Plugin Support**.
3. Enviá el sinte por L y la voz por R al mismo submix, y cargá **Argevox Gerät** como insert en ese submix.

El [manual en español](docs/manual.md) explica el ruteo, los controles en alemán, cómo ajustar el umbral y cómo diagnosticar una fuga de la portadora.


## Próximas versiones

- **0.2.2** — actualización del empaquetado con las herramientas actuales de OpenMPC, créditos de Argerax y registro de la instalación verificada desde el Plugin Manager. Pendiente de publicación.
- **1.0.0-beta.1** — desarrollo del nuevo motor, guiado por el manual y los esquemas del Sennheiser VSM-201, para mejorar la articulación, la inteligibilidad y el ajuste de niveles. Pendiente de implementación y pruebas en MPC.

## Código y compilación

Usá el [MPC Plugin Builder SDK de inMusic](https://github.com/inmusic-software/mpc-plugin-builder-sdk/tree/febd2ca5bd9eb16c4a412837196e145566bc981b). Clonalo y ubicá los archivos de este repositorio directamente en `plugins/Argevox Gerät/`. Desde la raíz del SDK:

```sh
python tools/build.py "plugins/Argevox Gerät"
python tools/package.py "plugins/Argevox Gerät"
```

El SDK adjunta la interfaz desde `skins/`. La compilación ARM 32/64 de esta versión también se verificó con Clang y el sysroot Arm GNU 13.3. Ese método requiere añadir `compat/glibc-strtoul.cpp` y enlazar con `-Wl,--wrap=__isoc23_strtoul` para evitar la dependencia de glibc 2.38. El binario de 32 bits publicado requiere glibc 2.32 y el de 64 bits, 2.35.

## Licencia

MIT. Argerax es el fabricante y autor del código y los recursos originales de Argevox Gerät. Los componentes reutilizados del SDK conservan los avisos de inMusic. El código usa el framework, los filtros y el suavizado del SDK de inMusic. Consultá [LICENSE](LICENSE).
