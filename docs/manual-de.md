# Argevox Gerät — 1.0.0 Beta 2

Entwicklung und Hersteller Argerax. Interne Plugin-Version 1.0.0, veröffentlicht als Beta 2.

[English](manual-en.md) · [Español](manual-es.md)

## Installation und erster Start

Erforderlich sind MPC OS 3.10.1 oder neuer sowie die aktivierten Einstellungen Preferences → Enhancements (Beta) → Enable Enhancements und External Plugin Support. Für die manuelle Installation das entsprechende ZIP entpacken und den Ordner `Argerax - VST - Argevox Gerät` in `VST2 Plugins` im Stammverzeichnis des Laufwerks kopieren. Anschließend die MPC neu starten oder die externen Plugins entsprechend den Möglichkeiten des Geräts neu einlesen. Die architekturspezifischen ZIP-Dateien sind für den OpenMPC Plugin Manager bestimmt.

Diese Beta verwendet dieselbe Plugin-Kennung wie 0.2.1 und ersetzt deren Installation. Das bisherige Installationspaket und eine Kopie der Projekte für Vergleiche aufbewahren. Kennungen und Reihenfolge der ersten 49 Parameter bleiben erhalten. Die neue Klangerzeugung verändert den Klang, daher können ältere Pegel- und Schwelleneinstellungen eine Anpassung benötigen. Beta 2 verwendet dieselben DSP-Binärdateien wie Beta 1 und ordnet die Oberfläche auf fünf Seiten neu.

## Audio-Routing

Den Synthesizer oder nativen MPC-Oszillator vollständig nach links und das Mikrofon vollständig nach rechts pannen. Beide Signale an denselben Stereo-Submix senden und Argevox dort als Insert laden. Das Mikrofon-Monitoring auf In stellen. L führt das Trägersignal, R das Sprachsignal als Modulator. Die bearbeitete Ausgabe ist ein mittiges Monosignal, das auf beide Ausgangskanäle geschrieben wird.

Für einen ersten Vergleich

1. **Klar 201** laden. Stimme und Träger auf 0 dB, die Synthesebänder auf 0 dB und Entzischung auf 0 stellen. Auf Mini D bei geöffnetem Filter einen tiefen Sägezahnton halten. Auch ein nativer MPC-Oszillator eignet sich als Träger.
2. **Schwelle vorübergehend auf −80 dBFS** stellen. Dadurch schließt das Gate nicht. Anschließend die Schwelle so weit erhöhen, dass Hintergrundgeräusche das Gate nicht mehr öffnen, ohne Silbenanfänge abzuschneiden.
3. Normal sprechen. **Stimme AGC** und **Träger AGC** regeln die internen Pegel mit begrenzter Verstärkung. Ausgang bestimmt den Abhörpegel.
4. Mit **Dunkle Stimme** vergleichen. Negative Formanten-Werte verschieben die spektralen Resonanzen nach unten, während die gespielten Synthesizernoten erhalten bleiben. Dunkelheit senkt die oberen Bänder zunehmend ab.
5. Bei schwachen Konsonanten den Regler Konsonanten schrittweise erhöhen. Klarheit mischt bei stimmlosen Lauten einen kleinen hochpassgefilterten Anteil des echten Mikrofonsignals hinzu. Der Anfangswert ist 0, damit zunächst die reine Vocoder-Synthese beurteilt werden kann.

## Oberfläche und Bedienelemente

Die Parameternamen sind in allen Sprachfassungen dieses Handbuchs deutsch.

| Seite | Bedienelemente und Funktion |
| --- | --- |
| STIMME | Stimme, Träger, Konsonanten und Reaktion; Schwelle, Entzischung, Stimme AGC und Träger AGC; Betriebsart, Stimmart, Tonquelle und Tonhöhe. Drei Reihen mit je vier Reglern. |
| KLANG | Ausgang, Formanten, Dunkelheit und Klarheit; Kontur, Rosa Rauschen, Sprachanteil und Pausenpegel. |
| BÄNDER | Alle 20 Synthesepegel gemeinsam, in zwei Reihen mit je zehn berührungsempfindlichen Reglern, Mittenfrequenzen und Aktivitätsanzeigen. Regelbereich −18 bis +18 dB. |
| SPRACHE | Alle 20 Anteile der gefilterten Sprache gemeinsam, in zwei Zehnerreihen. Sie wirken auf Multifilter und Sprachanteil. |
| PAUSEN | Pausenmodus, Pausenpegel, Schwelle und Ausgang; darunter zehn Pausenpaar-Regler für jeweils zwei benachbarte Bänder. |

Jede Seite hat mindestens vier zugewiesene Q-Links. Die Bedienelemente sind reihenweise gruppiert. Auf BÄNDER und SPRACHE steuern vier Vierergruppen die Bänder 1–8 und 11–18. Die Bänder 9, 10, 19 und 20 werden über den Touchscreen bedient. Alle 20 Regler sind gleichzeitig sichtbar. Ein Doppeltippen öffnet die vergrößerte Werteingabe. Das Berühren eines zugewiesenen Reglers wählt die zugehörige Q-Link-Gruppe.

### Betriebsart

- **Vocoder** überträgt die spektralen Hüllkurven auf das Anregungssignal und mischt die gewählten Sprachanteile hinzu.
- **Multifilter** führt die Sprache durch die Analysefilter und die 20 Sprache-Regler. Ein Trägersignal ist dafür nicht erforderlich.
- **Stimme** gibt das aufbereitete Mikrofonsignal zur Pegelkontrolle wieder.
- **Träger** gibt das aufbereitete Trägersignal wieder.

In den beiden Abhörbetriebsarten wird der gewählte Eingang zum Ausgang geführt. Ausgang und Spitzenpegelschutz bleiben aktiv.

### Stimmart und Tonquelle

- **Automatik** mischt tonale Anregung und Rauschen entsprechend der spektralen Verteilung der Sprache. Konsonanten bestimmt den automatischen Rauschanteil.
- **Tonhaft** verwendet tonale Anregung.
- **Rauschen** verwendet Rauschen in der gesamten Synthesefilterbank und ermöglicht artikuliertes Flüstern.
- **Tonquelle** wählt Extern L, Säge intern oder Puls intern. Die feste Frequenz des internen Oszillators wird mit Tonhöhe eingestellt. Als Effekt-Insert empfängt das Plugin keine MIDI-Noten.
- **Rosa Rauschen** ersetzt weißes Rauschen durch eine digitale Annäherung an rosa Rauschen.

### Formanten, Dunkelheit und Artikulation

**Formanten** verschiebt die Zuordnung der Analysehüllkurven zwischen den Synthesebändern um −12 bis +12 Halbtöne. Negative Werte betonen tiefere Resonanzen. **Dunkelheit** senkt obere Bänder zunehmend ab. **Kontur** schwächt geringe Analysepegel gegenüber den dominanten Bändern ab. **Reaktion** bestimmt Anstiegs- und Abklingzeiten der Hüllkurven; kleinere Werte folgen Änderungen schneller. **Entzischung** vermindert übermäßige Energie in den oberen Bändern bei stimmlosen Lauten. **Klarheit** fügt gefilterte echte Sprache hinzu und kann für die Beurteilung der vollständig synthetisierten Stimme auf 0 bleiben.

### Pausenfüllung

**Pausenmodus** bietet Aus, Global und Paarweise. Global wendet Pausenpegel auf die ganze Filterbank an. Paarweise berücksichtigt zusätzlich die zehn Pausenpaar-Regler. Das Trägersignal darf dabei absichtlich hörbar bleiben, wenn der Sprachdetektor schließt. Dafür ist ein aktiver Träger erforderlich. Bei Schwelle −80 bleibt der Detektor offen und die Pausenfüllung wird nicht wirksam. Für die normale Prüfung auf Stille Aus wählen.

### Presets

- **Klar 201** — Referenz für Artikulation, externer Träger, Dunkelheit 0.
- **Dunkle Stimme** — Formanten −4 Halbtöne, Dunkelheit 45%, externer Träger.
- **Tiefe Maschine** — interner Puls mit 85 Hz, Formanten −7, Dunkelheit 65%.
- **Flüstern** — artikuliertes rosa Rauschen. Anfangs wird die externe Quelle verwendet; für einen Versuch ohne Synthesizer eine interne Quelle wählen.
- **Multifilter** — gefilterte Sprache mit berührungsempfindlichen Anteilsreglern je Band.
- **Pausenbrücke** — globale Pausenfüllung mit gehaltenem externem Trägersignal.

## Fehlersuche

Bleibt Vocoder stumm, beide Eingangspegel und das L/R-Routing prüfen. Schwelle vorübergehend auf −80 und Tonquelle bei Mini D auf Extern L stellen. Ist nur ein Dauerton hörbar, kontrollieren, ob das Mikrofon auf R liegt und Pausenmodus auf Aus steht. Bei dumpfer Sprache mit Klar 201, Entzischung 0, Dunkelheit 0 und neutralen Bandpegeln beginnen. Eine erfolgreiche Einstellung als Preset oder MPC-Projekt speichern.

## VSM-201 als Referenz und Umfang dieser Beta

Die Klangerzeugung ist eine funktionale digitale Interpretation der im Herstellerhandbuch beschriebenen Blöcke. Enthalten sind 20 Kanäle, Hüllkurvenanalyse, AGC, tonale und rauschbasierte Anregung, automatische Stimmarterkennung, Zumischung gefilterter Sprache, Multifilter, Eingangsabhöre sowie globale oder paarweise Pausenfüllung.

Die aus Abbildung 8 des Herstellers näherungsweise abgelesenen und gerundeten Mittenfrequenzen betragen 140, 230, 330, 450, 580, 700, 850, 1000, 1170, 1360, 1570, 1830, 2140, 2500, 2900, 3400, 4000, 4800, 5800 und 7500 Hz.

Jeder Analyse- und Synthesekanal verwendet drei kaskadierte digitale Resonatoren. Nach der Gleichrichtung folgen zwei Glättungsstufen mit bandabhängigen Zeiten. Die Modulation erfolgt vor den Synthesefiltern. Stimmarterkennung, AGC, Zeitkonstanten, rosa Rauschen und Gütewerte sind angenäherte digitale Eigenentwicklungen von Argerax. Eine Bauteilemulation und ein kalibrierter Vergleich mit einem physischen VSM-201 wurden bisher nicht durchgeführt.

Die Stereoanpassung besitzt zwei Eingänge und zwei Ausgänge. Sie stellt die CV-Anschlüsse, die 20 physischen Hüllkurvenausgänge, den unabhängigen Eingang für stimmlose Anregung und die externen Hardwareverbindungen des Originals nicht bereit. Hüllkurven besitzen Aktivitätsanzeigen auf dem Bildschirm. Dunkelheit und Klarheit sind Erweiterungen von Argerax.

## Validierung und Hardwaretests

Automatische Prüfungen umfassen das Laden, den Zustand aller 93 Parameter, sechs Presets, Blockgrößen von 32 bis 512 Samples, endliche Ausgangswerte, In-place-Verarbeitung, Stille, Wiederherstellung der 49 veröffentlichten Parameter, Abhörfunktionen, Multifilter, artikuliertes Rauschen, Pausenfüllung und spektrale Verschiebungen durch Formanten. Beide ARM-Pakete bestehen den aktuellen OpenMPC-Katalogvalidator ohne Warnungen.

Der Autor meldete erfolgreiche Wiedergabe von Beta 1 auf MPC One+ mit MPC OS 3.10.1, einschließlich Akkorden und dunklen Stimmen, und hob Klar 201 hervor. Beta 2 verwendet genau dieselben DSP-Binärdateien. Die CPU-Auslastung und die Installation dieser neuen Beta über OpenMPC sind noch nicht gemessen beziehungsweise bestätigt. Die Hardwaremeldung ist ein Anwenderbericht des Autors; ein kalibrierter Vergleich mit dem historischen Gerät steht aus.

## Quellen und Lizenz

- [Originalhandbuch und Schaltpläne von Sennheiser](https://rubidium.se/~magnus/synths/companies/sennheiser/)
- [Abbildung 8, Frequenzgänge der Kanalfilter](https://rubidium.se/~magnus/synths/companies/sennheiser/Sennheiser_VSM201_manual_il8_200.jpg)
- [B101–B110, Schaltung der Spektralkanäle](https://rubidium.se/~magnus/synths/companies/sennheiser/Sennheiser_VSM201_schematic_b101_b110_200.jpg)

Copyright © 2026 Argerax für eigene Beiträge. MIT. Wiederverwendete Komponenten des MPC Plugin Builder SDK behalten die inMusic-Hinweise. Siehe LICENSE.
