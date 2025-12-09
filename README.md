# DC Motor Control System with Dual Atmega32

A master-slave motor control system using two Atmega32 microcontrollers communicating via I2C.

## System Architecture

```
┌─────────────────┐         I2C Bus         ┌─────────────────┐
│     MCU1        │◄──────────────────────►│      MCU2       │
│    (Master)     │      SDA + SCL          │     (Slave)     │
│                 │                         │                 │
│  • Keypad input │   Sends: mode,          │  • Motor driver │
│  • User LCD     │   setpoint, emergency   │  • Encoder read │
│  • Mode select  │ ──────────────────────► │  • Status LCD   │
│  • Emergency X  │                         │  • PID control  │
└─────────────────┘                         └─────────────────┘
```

## Hardware Components

| Component | Quantity | Purpose |
|-----------|----------|---------|
| ATMEGA32 | 2 | MCU1 (master) and MCU2 (slave) |
| LM016L (16x2 LCD) | 2 | One for each MCU |
| MM74C922 | 1 | 16-key keypad encoder |
| 4x4 Keypad | 1 | User input |
| DC Motor with Encoder | 1 | The motor to control |
| L293D | 1 | H-Bridge motor driver |

## Building

```bash
make mcu1    # Build MCU1 (Master) firmware
make mcu2    # Build MCU2 (Slave) firmware
make all     # Build both
```

## Project Structure

```
├── src/
│   ├── mcu1/          # Master MCU firmware
│   ├── mcu2/          # Slave MCU firmware
│   └── common/        # Shared libraries
└── proteus/           # Schematic documentation
```
