# VGM YM2203C Player on RP2350

This repository provides a VGM player library for YM2203C, running on RP2350. It allows playback of VGM format data for the YM2203C sound chip.

## Features

- Supports playback of YM2203C VGM data
- Designed for RP2350 microcontroller
- Includes a tool to convert VGM files into C++ header files
- Open-source and easy to integrate into other projects

## Installation

1. Clone this repository:
   ```sh
   git clone https://github.com/yeisapporo/VGM-YM2203C-_Player_on_RP2350.git
   ```
2. Build the project according to the instructions in `BUILD.md` (if applicable).

## Usage

### Playing a VGM File

1. Convert a VGM file to a C++ header file using the provided tool.
2. Include the generated header file in your project.
3. Initialize the YM2203C player and start playback.

### Converting VGM to C++ Header

A tool for converting VGM files into C++ header format is located in the `tool` directory.

#### Steps:

1. Navigate to the `tool` directory:
   ```sh
   cd tool
   ```
2. Run the conversion script:
   ```sh
   python dump_vgm.py input.vgm
   ```
3. The generated header file `vgm_data.hpp` will contain the VGM data as a C++ byte array.

### Sample Code

A sample usage of the VGM player can be found in `usage.cpp`. Below is an overview of the necessary components:

1. **Include the Necessary Headers:**

   ```cpp
   #include <Arduino.h>
   #include "vgm_data.hpp"  // The generated VGM data header (rename the file if needed.)
   #include "ym2203c.hpp"
   ```

2. **Initialize the Player:**

   ```cpp
   YM2203C sound;
   sound.init(HC595_SCK, HC595_RCK, HC595_SI, YM2203_CS1, YM2203_WR, YM2203_A0);
   sound.repeatied_timer_us(22); // Set playback interval
   ```

3. **Playback Handling:**

   The `vgm_play()` function is executed inside the timer interrupt handler, ensuring accurate timing for playback.

   ```cpp
   static void timer_handler(void) {
       sound.vgm_play();
   }
   ```

### Understanding `ym2203c.hpp`

- `YM2203C::init(...)` initializes the hardware and sets pin modes.
- `YM2203C::vgm_play()` reads VGM data and sends it to the YM2203C chip.
- `YM2203C::write_ym2203()` writes data to the chip.
- `YM2203C::repeatied_timer_us()` sets up a periodic timer for playback control.
- The timer interrupt ensures playback occurs at the correct interval, eliminating the need for a manual loop call.

## Demonstration

A video of the system running on actual hardware can be seen here:
https://www.youtube.com/watch?v=wJDZg0ryA-E
