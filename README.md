# Weighing Scale Using Load Cells

> **EN2091 Analog Project** — Department of Electronic and Telecommunication Engineering, University of Moratuwa, Sri Lanka

---

## 👥 Team Morpheus 

| Name | GitHub Link |
|---|---|
| Gunarathne K.T.M.B |  |
| Jayaweera N.S | https://github.com/NisalJayaweera |
| Meedeniya M.M.H | https://github.com/manjarimeedeniya |
| Ranawaka R.A.C.D |  |

---

## 📋 Project Overview

An electronic weighing scale designed and implemented using a **strain gauge load cell**. The system captures the low-level differential voltage produced by the load cell, amplifies and filters it through an analog signal conditioning circuit, and displays the corresponding weight on a **16×2 LCD screen** via an ESP32 microcontroller.

**Key Specifications:**
- Maximum measuring capacity: **10 kg**
- Power input: **230V AC mains**
- Full-scale output voltage: **3V** (at 10 kg)
- Display: **16×2 LCD** (via I²C interface)
- Microcontroller: **ESP-WROOM-32E (ESP32)**

---

## 📸 Final Product
<p align="center">
  <img src="docs/images/final_product.jpg" alt="Weighing Scale - Final Product" width="60%"/>
</p>

Replace the image path above with your actual photo filename after adding it to the docs/images/ folder.


---

## 🏗️ System Architecture

```
230V AC Supply
      │
      ▼
Step-Down Transformer (230V → 13.5V AC)
      │
      ▼
Power Supply Circuit (Delon Doubler + Regulators)
      │
      ├──► +12V, -12V (for op-amps & load cell excitation)
      ├──► +5V  (for LCD)
      └──► +3.3V (for ESP32)

Load Cell (Wheatstone Bridge)
      │
      ▼
INA114 Instrumentation Amplifier (Gain ≈ 100)
      │
      ▼
Zero Adjust Circuit (TL084 op-amp + potentiometer)
      │
      ▼
Span Adjust Circuit (TL084 op-amp, variable gain)
      │
      ▼
3rd Order Sallen-Key Low-Pass Filter (10 Hz cutoff, TL072)
      │
      ▼
ESP32 ADC → Weight Calculation → 16×2 LCD Display
```

---

## ⚙️ Circuit Design

### Load Cell
- Type: Strain-gauge based, Wheatstone bridge configuration
- Capacity: 20 kg (used up to 10 kg)
- Rated output: 2.0 ± 0.15 mV/V
- Excitation voltage: 12V DC

### Amplifier Circuit
| Stage | Component | Purpose |
|---|---|---|
| Instrumentation Amplifier | INA114 | Amplifies differential load cell output (gain ≈ 100), high CMRR (~115 dB) |
| Zero Adjust | TL084 + 100kΩ potentiometer | Cancels offset voltage at zero load |
| Span Adjust | TL084 + 100kΩ potentiometer | Calibrates full-scale output to 3V at 10 kg |
| Low-Pass Filter | TL072 (3rd order Sallen-Key) | Cuts off noise above 10 Hz |

### Power Supply Circuit
| Regulator | Output | Powers |
|---|---|---|
| LM7812 | +12V | Load cell, INA114, op-amps |
| LM7912 | -12V | Op-amps (dual supply) |
| LM7805 | +5V | LCD display |
| AMS1117 | +3.3V | ESP32 chip |

The Delon Doubler circuit is used to generate the dual-supply (±) voltage from the single-rail transformer output.

---

## 🧰 Components List

### Amplifier Circuit
| Component | Quantity |
|---|---|
| INA114 Instrumentation Amplifier IC | 1 |
| TL084 Quad Op-Amp IC | 1 |
| TL072 Dual Op-Amp IC | 1 |
| 470Ω Resistor | 1 |
| 10kΩ Resistors | 8 |
| 100kΩ Resistor | 1 |
| 110kΩ Resistor | 1 |
| 11kΩ Resistor | 1 |
| 33kΩ Resistor | 1 |
| 100kΩ Variable Resistors (Potentiometers) | 2 |
| 0.1µF Capacitors | 5 |
| 0.68µF Capacitor | 1 |
| 1.5µF Capacitor | 1 |
| Push Buttons | 2 |
| JST Wire Connectors 2.54mm (4-pin) | 3 |
| JST Wire Connectors 2.54mm (5-pin) | 1 |

### Power Supply Circuit
| Component | Quantity |
|---|---|
| 230V AC to 13.5V AC Step-Down Transformer | 1 |
| 1N4007 Diodes | 2 |
| 50V 1000µF Capacitors | 2 |
| 50V 470µF Capacitors | 2 |
| 50V 100µF Capacitors | 2 |
| 50V 10µF Capacitors | 2 |
| LM7812 +12V Regulator | 1 |
| LM7912 -12V Regulator | 1 |
| LM7805 +5V Regulator | 1 |
| AMS1117 3.3V Regulator | 1 |
| Switch | 1 |
| LED | 1 |
| JST Wire Connectors 2.54mm (2-pin) | 3 |
| JST Wire Connectors 2.54mm (5-pin) | 1 |

### Other Components
| Component | Quantity |
|---|---|
| 20kg Strain-Gauge Load Cell | 1 |
| ESP-WROOM-32E Microcontroller | 1 |
| 16×2 LCD Display | 1 |
| IIC/I²C Serial Interface Module for LCD | 1 |
| 3-pin AC Power Base | 1 |
| Power Cable | 1 |

---

---

## 🔧 Calibration Procedure

1. **Zero Adjustment** — With no load on the scale, adjust the zero-adjust potentiometer until the LCD reads `0.00 kg`.
2. **Span Adjustment** — Place a known 10 kg reference weight on the platform. Adjust the span-adjust potentiometer until the display reads `10.00 kg`.
3. The system is now calibrated across the full measurement range.

---

## 💻 Firmware

The ESP32 firmware reads the analog voltage output from the signal conditioning circuit via its built-in ADC, applies the calibration mapping, and sends the weight value to the 16×2 LCD over I²C.

**Upload Instructions:**
1. Install the [Arduino IDE](https://www.arduino.cc/en/software) and add ESP32 board support.
2. Install the `LiquidCrystal_I2C` library via the Library Manager.
3. Open `firmware/main.ino`, select your board (`ESP32 Dev Module`) and COM port.
4. Upload the sketch.

---

## 🚧 Challenges Encountered

- **Capacitor availability** — 0.68µF and 1.5µF electrolytic capacitors were unavailable locally. Ceramic equivalents were used, but their lead spacing required manual wire extensions to fit the PCB footprint.
- **Electromagnetic interference** — After full assembly, the amplifier output showed an unexpected ~60V peak-to-peak sine wave. This was traced to electromagnetic radiation from the mains transformer coupling into nearby signal wires. The fix involved additional transformer shielding with a metal plate and shortening signal wire runs.
- **Platform deformation** — The initial wooden weighing platform deformed under load and made contact with the base, introducing measurement errors. It was replaced with a rigid acrylic plate with a larger clearance gap.

---

## 🔭 Future Improvements

- **Battery operation** — Replace mains power with a low-power design for portability.
- **Wireless connectivity** — Add Bluetooth or Wi-Fi (ESP32 already supports this) for remote monitoring and data logging to a mobile app or PC.

---

## 👨‍💼 Contributions

| Member | Contributions |
|---|---|
| Gunarathne K.T.M.B  | ESP32 firmware, LCD integration, soldering, assembly, component selection |
| Jayaweera N.S  | PCB design, soldering, assembly, component selection |
| Meedeniya M.M.H  | Circuit design, enclosure design, assembly, component selection |
| Ranawaka R.A.C.D  | Enclosure span design, soldering, assembly, component selection |

---

## 🙏 Acknowledgements

We sincerely thank **Dr. Sampath Perera** and supervisor **Mr. Viruthshaan** for their guidance throughout the project. We also appreciate the feedback from **Dr. Jayathu Samarawickrama**, **Dr. Pranjeevan Kulasingham**, **Dr. Wageesha N. Manamperi**, and **Mr. Lathika Wathsara** during the project evaluation. Thanks to the academic and non-academic staff of the ENTC Department, University of Moratuwa, for their support and facilities.

---

## 📚 References

1. INA114 Datasheet — https://www.alldatasheet.com/datasheet-pdf/view/847617/TI1/INA114.html
2. TL084 Datasheet — https://www.alldatasheet.com/datasheet-pdf/view/25385/STMICROELECTRONICS/TL084.html
3. TL072 Datasheet — https://www.alldatasheet.com/datasheet-pdf/view/28775/TI/TL072.html
4. LM7812 Datasheet — https://www.alldatasheet.com/datasheet-pdf/pdf/838008/TI1/LM7812.html

---
