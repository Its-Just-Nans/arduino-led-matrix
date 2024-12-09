#!/bin/bash

TYPE="arduino:avr:nano:cpu=atmega328old"
PORT="/dev/ttyUSB0"

arduino-cli compile --fqbn "$TYPE" sketch-arduino-led-matrix
arduino-cli upload -p "$PORT" --fqbn "$TYPE" sketch-arduino-led-matrix

python main.py
