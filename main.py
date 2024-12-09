#!/bin/env python

"""arduino led matrix"""

import time
from json import load
import threading
import serial


alphabet = {
    "a": [255, 195, 189, 189, 129, 189, 189, 189],
    "b": [255, 193, 189, 189, 193, 189, 189, 193],
    "c": [255, 195, 189, 189, 253, 189, 189, 195],
    "d": [255, 225, 221, 189, 189, 189, 221, 225],
    "e": [255, 193, 253, 253, 193, 253, 253, 193],
    "f": [255, 193, 253, 253, 225, 253, 253, 253],
    "g": [255, 195, 189, 189, 253, 141, 189, 195],
    "h": [255, 189, 189, 189, 129, 189, 189, 189],
    "i": [255, 193, 247, 247, 247, 247, 247, 193],
    "j": [255, 129, 239, 239, 239, 237, 237, 243],
    "k": [255, 221, 237, 245, 249, 245, 237, 221],
    "l": [255, 253, 253, 253, 253, 253, 253, 193],
    "m": [255, 189, 153, 165, 189, 189, 189, 189],
    "n": [255, 189, 185, 181, 173, 157, 189, 189],
    "o": [255, 195, 189, 189, 189, 189, 189, 195],
    "p": [255, 193, 189, 189, 193, 253, 253, 253],
    "q": [255, 195, 189, 189, 189, 173, 157, 195],
    "r": [255, 193, 189, 189, 193, 245, 237, 221],
    "s": [255, 131, 253, 253, 195, 191, 191, 193],
    "t": [255, 129, 247, 247, 247, 247, 247, 247],
    "u": [255, 189, 189, 189, 189, 189, 189, 195],
    "v": [255, 189, 189, 219, 219, 219, 231, 231],
    "w": [255, 189, 189, 165, 165, 165, 219, 219],
    "x": [255, 189, 219, 231, 231, 219, 189, 189],
    "y": [255, 189, 219, 231, 247, 247, 247, 247],
    "z": [255, 129, 191, 223, 239, 247, 251, 129],
}


def get_letter_matrix(letter):
    """get a letter representation in a 8x8 matrix"""
    if letter == "":
        return [0, 0, 0, 0, 0, 0, 0, 0]
    if letter in alphabet:
        return alphabet[letter]
    return [255, 255, 255, 255, 255, 255, 255, 255]


def convert_string_to_bytes(string):
    """convert a string to bytes"""
    return bytes([int(string[i : i + 8][::-1], 2) for i in range(0, len(string), 8)])


def get_matrix_of(letter) -> bytes:
    """get a matrix of a letter as bytes"""
    letter = get_letter_matrix(letter)
    return bytes(letter)


def handle_arduino(message, port="/dev/ttyUSB0", baudrate=115200):
    """handle the arduino and send the message"""
    arduino = serial.Serial(port=port, baudrate=baudrate, timeout=0.01)
    is_array = isinstance(message, list)
    current_letter_index = 0
    while True:
        received = None
        try:
            received = arduino.readline().decode()
        except UnicodeDecodeError:
            continue
        if received is None or received == "":
            continue
        if "ASKING_VALUE" in received:
            letter = message[current_letter_index]
            if is_array:
                bytes_to_send = letter
                letter = f"{letter} ({current_letter_index}/{len(message)})"
            else:
                bytes_to_send = get_matrix_of(letter)
            print(f"sending {letter}: {bytes_to_send}")
            arduino.write(bytes_to_send)
        elif "ERROR_READING" in received or "READ_NOTHING" in received:
            print(f"received error '{received}'")
        else:
            received = received.replace("\r", "").replace("\n", "\\n")
            print(f"received {received}")
            current_letter_index += 1
            if current_letter_index >= len(message):
                break
        time.sleep(0.5)
    arduino.close()


def cli_close():
    """quit program"""
    message = input("Do you want to close the program? (y/n)")
    if message == "y":
        print("Program closed")
        exit(0)


def reverse_bits(byte):
    """reverse the bits of a byte"""
    reversed_byte = 0
    for _i in range(8):  # 8 bits in a byte
        reversed_byte = (reversed_byte << 1) | (byte & 1)
        byte >>= 1
    return reversed_byte


def show_matrix(matrix):
    for one_byte in matrix:
        reversed_byte = reverse_bits(one_byte)
        visual = "".join(
            ("⬛" if (reversed_byte >> i) & 1 else "⬜" for i in range(7, -1, -1))
        )
        print(f"{visual} {one_byte}")


def cli_show():
    """show the matrix of a letter"""
    message = input("Enter a letter to show: ")
    matrix = get_matrix_of(message)
    print(get_letter_matrix(message))
    print(matrix)
    show_matrix(matrix)


def cli_send():
    """send a letter to the arduino"""
    message = input("Enter a message to send: ")
    message = message.lower() + " "
    handle_arduino(message)


def cli_json_file():
    """send a file to the arduino"""
    file_path = input("Enter a file path to send: ")
    data = []
    with open(file_path, "r", encoding="utf-8") as file:
        data = load(file)
    array_bytes = list(map(convert_string_to_bytes, data))
    print(array_bytes)
    handle_arduino(array_bytes)


def setInterval(func, time):
    e = threading.Event()
    while not e.wait(time):
        func()


count = 0


def cli_json_console():
    """send a file to the arduino"""

    file_path = input("Enter a file path to send: ")
    file_path = "raw_bad_apple.json"
    data = []
    with open(file_path, "r", encoding="utf-8") as file:
        data = load(file)
    array_bytes = list(map(convert_string_to_bytes, data))
    print(array_bytes)

    def show():
        global count
        show_matrix(array_bytes[count])
        count += 1
        print("\033[H\033[J", end="")

    setInterval(show, 1 / 60)


COMMANDS = {
    "close": cli_close,
    "exit": lambda: exit(0),
    "show": cli_show,
    "send": cli_send,
    "json": cli_json_file,
    "console": cli_json_console,
}


def main():
    """main func"""

    while True:
        try:
            message = input(f"Enter a command: {list(COMMANDS.keys())}\n")
            if message not in COMMANDS:
                print("Command not found")
                continue
            COMMANDS[message]()
        except KeyboardInterrupt:
            pass


if __name__ == "__main__":
    main()
