# Proteus Schematic Documentation

## DC Motor Control System - Dual Atmega32

This document describes the hardware connections for the DC Motor Control System using two Atmega32 microcontrollers.

## Component List

| Component | Part Number | Quantity | Value/Rating | Notes |
|-----------|------------|----------|--------------|-------|
| Microcontroller | ATMEGA32 | 2 | 16MHz | MCU1 (Master), MCU2 (Slave) |
| LCD Display | LM016L | 2 | 16x2 Character | One per MCU |
| Keypad Encoder | MM74C922 | 1 | 16-key encoder | With debounce |
| Keypad | Matrix 4x4 | 1 | 16 keys | Standard matrix keypad |
| Motor Driver | L293D | 1 | H-Bridge | Or L298N alternative |
| DC Motor | - | 1 | 12V | With quadrature encoder |
| Encoder | Rotary Encoder | 1 | 100 PPR | Integrated with motor |
| Resistor | - | 2 | 4.7kΩ | I2C pull-ups (SDA, SCL) |
| Crystal | - | 2 | 16MHz | External oscillator |
| Capacitor | - | 4 | 22pF | Crystal load capacitors |
| Capacitor | - | 2 | 100nF | Decoupling for MCUs |
| Power Supply | - | 1 | 5V/12V | 5V for logic, 12V for motor |

## MCU1 (Master) - Pin Connections

### I2C Interface
| Pin | Function | Connection |
|-----|----------|------------|
| PC0 | SDA | I2C Data (with 4.7kΩ pull-up to VCC) |
| PC1 | SCL | I2C Clock (with 4.7kΩ pull-up to VCC) |

### LCD Display #1
| Pin | Function | Connection |
|-----|----------|------------|
| PB0 | RS | LCD RS (Register Select) |
| PB1 | EN | LCD Enable |
| PB4 | D4 | LCD Data 4 |
| PB5 | D5 | LCD Data 5 |
| PB6 | D6 | LCD Data 6 |
| PB7 | D7 | LCD Data 7 |

### MM74C922 Keypad Encoder
| Pin | Function | Connection |
|-----|----------|------------|
| PA0 | DA | Data Available (output from MM74C922) |
| PA1 | D0 | Data bit 0 |
| PA2 | D1 | Data bit 1 |
| PA3 | D2 | Data bit 2 |
| PA4 | D3 | Data bit 3 |

### Power and Clock
| Pin | Function | Connection |
|-----|----------|------------|
| VCC | Power | +5V |
| GND | Ground | GND |
| XTAL1 | Crystal | 16MHz crystal |
| XTAL2 | Crystal | 16MHz crystal |
| AVCC | Analog VCC | +5V (with filter) |
| AREF | Analog Ref | 100nF to GND |

## MCU2 (Slave) - Pin Connections

### I2C Interface
| Pin | Function | Connection |
|-----|----------|------------|
| PC0 | SDA | I2C Data (with 4.7kΩ pull-up to VCC) |
| PC1 | SCL | I2C Clock (with 4.7kΩ pull-up to VCC) |

### LCD Display #2
| Pin | Function | Connection |
|-----|----------|------------|
| PB0 | RS | LCD RS (Register Select) |
| PB1 | EN | LCD Enable |
| PB4 | D4 | LCD Data 4 |
| PB5 | D5 | LCD Data 5 |
| PB6 | D6 | LCD Data 6 |
| PB7 | D7 | LCD Data 7 |

### Motor Control (L293D H-Bridge)
| Pin | Function | Connection |
|-----|----------|------------|
| PD5 | PWM (OC1A) | L293D Enable (EN1,2) - Speed control |
| PD6 | DIR1 | L293D Input 1 (IN1) |
| PD7 | DIR2 | L293D Input 2 (IN2) |

### Encoder Interface
| Pin | Function | Connection |
|-----|----------|------------|
| PD2 | INT0 | Encoder Channel A (pulse input) |

### Power and Clock
| Pin | Function | Connection |
|-----|----------|------------|
| VCC | Power | +5V |
| GND | Ground | GND |
| XTAL1 | Crystal | 16MHz crystal |
| XTAL2 | Crystal | 16MHz crystal |
| AVCC | Analog VCC | +5V (with filter) |
| AREF | Analog Ref | 100nF to GND |

## I2C Bus Wiring

The I2C bus connects MCU1 and MCU2:

```
MCU1 PC0 (SDA) ──────┬──── 4.7kΩ ──── VCC (+5V)
                     │
MCU2 PC0 (SDA) ──────┘

MCU1 PC1 (SCL) ──────┬──── 4.7kΩ ──── VCC (+5V)
                     │
MCU2 PC1 (SCL) ──────┘
```

**Important Notes:**
- Pull-up resistors (4.7kΩ) are required on both SDA and SCL lines
- Keep I2C bus length short (< 1 meter) for reliable 100kHz operation
- MCU2 slave address is programmed as 0x42

## MM74C922 to Keypad Connection

The MM74C922 keypad encoder connects to a standard 4x4 matrix keypad:

### MM74C922 Pin Configuration
| Pin | Function | Connection |
|-----|----------|------------|
| Y1-Y4 | Column drivers | Keypad columns 1-4 |
| X1-X4 | Row inputs | Keypad rows 1-4 |
| DA | Data Available | MCU1 PA0 |
| A-D | Data outputs | MCU1 PA1-PA4 (4-bit encoded key) |
| OSC | Oscillator | External capacitor or internal |
| VDD | Power | +5V |
| VSS | Ground | GND |

### Keypad Layout
```
     Col1  Col2  Col3  Col4
Row1  [1]   [2]   [3]   [A]
Row2  [4]   [5]   [6]   [B]
Row3  [7]   [8]   [9]   [C]  <- ON/C (Confirm)
Row4  [*]   [0]   [#]   [D]  <- X (Emergency)
                   ^
                   +--- (Toggle Mode)
```

## L293D H-Bridge Motor Driver

### L293D Pin Configuration
| Pin | Function | Connection |
|-----|----------|------------|
| EN1,2 (Pin 1) | Enable 1-2 | MCU2 PD5 (PWM) |
| IN1 (Pin 2) | Input 1 | MCU2 PD6 |
| OUT1 (Pin 3) | Output 1 | Motor terminal + |
| GND (Pin 4-5) | Ground | GND (heat sink) |
| OUT2 (Pin 6) | Output 2 | Motor terminal - |
| IN2 (Pin 7) | Input 2 | MCU2 PD7 |
| VCC2 (Pin 8) | Motor supply | +12V (motor power) |
| VCC1 (Pin 16) | Logic supply | +5V |

### Motor Control Logic
| DIR1 (PD6) | DIR2 (PD7) | Motor Action |
|------------|------------|--------------|
| 0 | 0 | Stop (brake) |
| 1 | 0 | Forward |
| 0 | 1 | Backward |
| 1 | 1 | Stop (brake) |

Speed is controlled by PWM duty cycle on EN1,2 (0-255).

## Rotary Encoder Connection

The motor includes a quadrature encoder (100 PPR):

| Encoder Pin | Connection | Notes |
|-------------|------------|-------|
| Channel A | MCU2 PD2 (INT0) | Pulse counting |
| Channel B | Not connected | Optional for direction |
| VCC | +5V | Encoder power |
| GND | GND | Ground |

**Notes:**
- Only Channel A is used for pulse counting
- INT0 triggers on rising edge for each pulse
- 100 pulses per revolution (PPR)
- Optional: Connect Channel B to PD3 (INT1) for direction sensing

## LCD Connections (Both MCU1 and MCU2)

Both LCDs connect identically to their respective MCUs:

| LCD Pin | Function | MCU Pin | Notes |
|---------|----------|---------|-------|
| 1 | VSS | GND | Ground |
| 2 | VDD | +5V | Power |
| 3 | V0 | Potentiometer | Contrast (10kΩ pot) |
| 4 | RS | PB0 | Register Select |
| 5 | R/W | GND | Write mode (tied low) |
| 6 | EN | PB1 | Enable |
| 7-10 | D0-D3 | Not connected | 4-bit mode |
| 11 | D4 | PB4 | Data 4 |
| 12 | D5 | PB5 | Data 5 |
| 13 | D6 | PB6 | Data 6 |
| 14 | D7 | PB7 | Data 7 |
| 15 | A | +5V | Backlight anode (330Ω resistor) |
| 16 | K | GND | Backlight cathode |

## Power Supply

### 5V Logic Supply
- Powers: Both MCUs, LCDs, MM74C922, Encoder
- Current: ~500mA typical
- Regulation: LM7805 or similar

### 12V Motor Supply
- Powers: DC motor via L293D
- Current: Depends on motor (1-2A typical)
- Regulation: Not critical, but filtering recommended

### Decoupling
- 100nF ceramic capacitor near each IC's VCC pin
- 10µF electrolytic on power supply rails

## Schematic Notes

1. **Reset Circuits**: Include 10kΩ pull-up resistors on both MCU RESET pins
2. **LED Indicators**: Optional status LEDs on spare GPIO pins
3. **ISP Headers**: Include 6-pin ISP programming headers for both MCUs
4. **Fuses**: Both MCUs should be configured for external 16MHz crystal
5. **ESD Protection**: Consider TVS diodes on I2C lines for robust operation

## Testing Checklist

- [ ] Verify 5V supply on all IC VCC pins
- [ ] Verify 12V on L293D motor supply
- [ ] Check I2C pull-ups are present (measure ~5V on idle bus)
- [ ] Test keypad encoder output with multimeter
- [ ] Verify LCD contrast adjustment works
- [ ] Program both MCUs via ISP
- [ ] Test I2C communication with logic analyzer
- [ ] Verify motor PWM signal with oscilloscope
- [ ] Check encoder pulses during manual motor rotation
- [ ] Test emergency stop function

## Recommended Tools

- **Oscilloscope**: For PWM and encoder signal verification
- **Logic Analyzer**: For I2C protocol debugging
- **Multimeter**: For power supply and continuity testing
- **AVR Programmer**: USBasp or similar for firmware upload
