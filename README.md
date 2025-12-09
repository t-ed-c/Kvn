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
                                                    │
                                                    ▼
                                            ┌───────────────┐
                                            │  Encoded DC   │
                                            │    Motor      │
                                            └───────────────┘
```

## Features

### MCU1 (Master) - User Interface
- **Keypad Input**: MM74C922 encoder with 4x4 matrix keypad
- **LCD Display**: 16x2 character LCD showing mode and setpoint
- **I2C Master**: Sends commands to MCU2 at 100kHz
- **Key Mappings**:
  - `+` (Hash key) - Toggle between M-Speed and M-Pos modes
  - `0-9` - Enter setpoint digits (up to 5 digits)
  - `C` - Confirm and send command to MCU2
  - `X` (D key) - Emergency stop
  
### MCU2 (Slave) - Motor Control
- **I2C Slave**: Receives commands at address 0x42
- **PWM Motor Control**: Timer1 generates ~976Hz PWM for L293D H-Bridge
- **Encoder Reading**: External interrupt (INT0) counts encoder pulses
- **PID Speed Control**: Configurable PID controller (Kp=1.0, Ki=0.1, Kd=0.05)
- **Position Control**: Revolution counting with encoder feedback
- **Emergency Stop**: Immediate motor shutdown with latch

## Hardware Components

| Component | Quantity | Purpose |
|-----------|----------|---------|
| ATMEGA32 | 2 | MCU1 (master) and MCU2 (slave) |
| LM016L (16x2 LCD) | 2 | One for each MCU |
| MM74C922 | 1 | 16-key keypad encoder |
| 4x4 Keypad | 1 | User input |
| DC Motor with Encoder | 1 | The motor to control |
| L293D or L298N | 1 | H-Bridge motor driver |
| 4.7kΩ Resistor | 2 | I2C pull-ups (SDA, SCL) |
| 16MHz Crystal | 2 | External oscillator for each MCU |

## Building

### Prerequisites
- AVR-GCC toolchain
- avr-libc library
- GNU Make

### Compilation
```bash
make mcu1    # Build MCU1 (Master) firmware
make mcu2    # Build MCU2 (Slave) firmware
make all     # Build both
make clean   # Clean build artifacts
```

### Programming
```bash
make flash-mcu1  # Flash MCU1 firmware
make flash-mcu2  # Flash MCU2 firmware
make flash       # Flash both (requires two programmers or sequential)
```

**Note**: Edit the `Makefile` to configure your programmer settings.

## Project Structure

```
├── src/
│   ├── mcu1/               # Master MCU firmware
│   │   ├── main.c          # Main application logic
│   │   ├── i2c_master.c/h  # I2C master driver
│   │   └── keypad.c/h      # MM74C922 keypad driver
│   ├── mcu2/               # Slave MCU firmware
│   │   ├── main.c          # Main application logic
│   │   ├── i2c_slave.c/h   # I2C slave driver
│   │   ├── motor.c/h       # PWM motor control
│   │   ├── encoder.c/h     # Encoder interrupt handling
│   │   └── pid.c/h         # PID controller
│   └── common/             # Shared libraries
│       ├── lcd.c/h         # LCD driver (4-bit mode)
│       └── protocol.h      # I2C communication protocol
├── proteus/                # Schematic documentation
│   └── README.md           # Pin assignments and wiring
├── Makefile                # Build system
└── README.md               # This file
```

## LCD Display States

### MCU1 Display Examples
```
Mode Selection:           Speed Entry:            Position Entry:
┌────────────────┐      ┌────────────────┐      ┌────────────────┐
│Mode: M-Speed   │      │Mode: M-Speed   │      │Mode: M-Pos     │
│Speed: ___  RPM │      │Speed: 500  RPM │      │Pos:  10    REV │
└────────────────┘      └────────────────┘      └────────────────┘
```

### MCU2 Display Examples
```
Speed Mode Running:     Position Mode:           Emergency:
┌────────────────┐      ┌────────────────┐      ┌────────────────┐
│M-Speed->500 RPM│      │M-Pos->10   REV │      │Emergency!      │
│Cur: 487    RPM │      │Cur: 7      REV │      │Stop!           │
└────────────────┘      └────────────────┘      └────────────────┘
```

## I2C Communication Protocol

The system uses a 5-byte packet structure:

```
┌────────┬────────┬────────┬────────┬────────┐
│ START  │  MODE  │ VALUE  │ VALUE  │  CHECK │
│ 0xAA   │ 1 byte │  HIGH  │  LOW   │  SUM   │
└────────┴────────┴────────┴────────┴────────┘
```

**MODE values**:
- `0x01` = M-Speed (value = RPM)
- `0x02` = M-Pos (value = Revolutions)
- `0xFF` = Emergency Stop

**Checksum**: Sum of MODE, VALUE_HIGH, and VALUE_LOW bytes

## Technical Specifications

- **MCU**: Atmega32 @ 16MHz (F_CPU = 16000000UL)
- **I2C Speed**: 100kHz standard mode
- **I2C Slave Address**: 0x42
- **PWM Frequency**: ~976Hz for motor control
- **Encoder**: 100 PPR (Pulses Per Revolution) - configurable via `ENCODER_PPR`
- **PID Constants**: Kp=1.0, Ki=0.1, Kd=0.05 - configurable via defines

## Configuration

Key parameters can be adjusted in the header files:

**src/mcu2/encoder.h**:
```c
#define ENCODER_PPR 100  // Pulses Per Revolution
```

**src/mcu2/pid.h**:
```c
#define PID_KP 1.0f   // Proportional gain
#define PID_KI 0.1f   // Integral gain
#define PID_KD 0.05f  // Derivative gain
```

**src/mcu2/main.c**:
```c
#define POSITION_CONTROL_SPEED 150  // PWM duty cycle for position mode
```

**src/common/protocol.h**:
```c
#define PROTOCOL_START_BYTE 0xAA
#define MCU2_I2C_ADDRESS 0x42
```

## Hardware Setup

See `proteus/README.md` for detailed:
- Pin connection diagrams
- I2C bus wiring (with pull-up resistors)
- MM74C922 to keypad connections
- L293D H-Bridge motor driver wiring
- Encoder connections
- LCD wiring for both MCUs
- Power supply requirements

## Usage

1. **Power On**: Both MCUs will initialize and display ready state
2. **Select Mode**: Press `+` to toggle between M-Speed and M-Pos
3. **Enter Setpoint**: Use `0-9` keys to enter desired RPM or revolutions
4. **Send Command**: Press `C` to confirm and send to MCU2
5. **Emergency Stop**: Press `X` at any time to stop the motor immediately

## Memory Usage

### MCU1 (Master)
- Program: 1494 bytes (4.5% of 32KB flash)
- Data: 92 bytes
- BSS: 9 bytes
- Total RAM: 101 bytes (4.9% of 2KB)

### MCU2 (Slave)
- Program: 4102 bytes (12.5% of 32KB flash)
- Data: 80 bytes
- BSS: 60 bytes
- Total RAM: 140 bytes (6.8% of 2KB)

## License

This project is open source and available for educational and commercial use.

## Contributing

Contributions are welcome! Please ensure:
- Code compiles without warnings with `-Wall -Wextra`
- Follow existing code style
- Test on hardware or in Proteus simulation
- Update documentation as needed
