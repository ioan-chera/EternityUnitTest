# EternityUnitTest
Unit tests for Eternity: github.com/team-eternity/eternity

## Eternity submodule
This project has Eternity as a submodule (from the aforementioned URL). To have your local copy fully configured, remember to also run `git submodule update --init --recursive` after you clone this repository.

## Setting up for Windows
You will need CMake to setup the Visual Studio 2019 project for Google Test. Use it to create a Visual Studio 2019 x86 (32-bit) build under `googletest/build/`. After this finishes successfully, you can open the `EternityUnitTest.sln` solution.

Currently everything is set up for 32-bit architecture on Windows.
