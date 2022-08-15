# ROM

The ROM is the first thing that runs when the chip goes off (power on/brown out/hard/soft/whatever) reset,
which then tries to load from the internal/external SPI flash,
and if failed then it enters the [download mode](dlmode/index.md) (that also can be triggered by pulling GPIO3 to ground),
and also the BLE stack with some peripheral drivers and generic utilities.

----

it's a **ROM**, a **Read-Only Memory**, not the *storage* and not a *firmware* that you flash!
so nowadays you probably use the terms MaskROM and OTP (One Time Programmable) to represent a ROM or PROM ??
but then, the *Flash* is technically an *EEPROM* - an (Electronically Erasable Programmable ROM)...
ahhhh whyyyy???
