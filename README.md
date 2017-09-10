# about bintracker

Bintracker is a free, open source, modular, cross-platform music editor for low-level sound drivers, and a visual front-end for the [Music Data Abstraction Language (MDAL)](https://utz82.github.io/MDAL). It is released under a 3-clause BSD license.

Currently, bintracker supports a number of modern 1-bit (beeper) music drivers for the Sinclair ZX Spectrum 48K home computer, including betaphase, PhaserX, PhaseSqueek, and Pytha. Additional drivers can easily be added via the MDAL configuration plug-in system.

Bintracker is confirmed to run on Windows and Linux. It should also be possible to build it on MacOS, though this is untested.

# downloads

Source, win32 executable, and documentation are available on the [github release page](https://github.com/utz82/bintracker/releases/latest).


# timeline

2017-09-10 first public beta release


# future plans

In the future, bintracker aims to support many more low-level sound drivers (through an upgraded MDAL standard), as well as supporting platforms other than the ZX Spectrum. Immediate next steps for the upcoming releases include:

- improving sound emulation
- stabilizing and improving the GUI
- cleaning up and optimizing the code
- upgrading the MDAL standard and rewriting libmdal
