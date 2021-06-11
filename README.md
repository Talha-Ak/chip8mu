# Chip8mu

> A rudimentary implementation of a Chip-8 emulator written in C++.

This project was a simple side project, created while I was learning C++. It implements a Chip-8 system, and is able to run Chip-8 programs and games.

This project contains two different ways for displaying graphics: [SDL](https://www.libsdl.org/) and [Qt](https://www.qt.io/).

Currently, there is no sound.

[This guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) was used as the high-level overview on the implementation detail of Chip-8.
Additionally, [this walkthrough](https://austinmorlan.com/posts/chip8_emulator/) was used to get display output working.
The actual execution loop and instructions were implemented by myself.

<img src="docs/chip8_1.png" alt="Tetris running on Chip-8" title="Tetris" width="400px">
<img src="docs/chip8_2.png" alt="Brick breaker running on Chip-8" title="Brick Breaker" width="400px">
<img src="docs/chip8_3.png" alt="Qt implementation of Chip-8" title="Qt version" width="400px">
<img src="docs/chip8_4.png" alt="Pong running on Qt Chip-8" title="Pong" width="400px">