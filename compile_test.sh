#!/bin/bash

echo "Compiling ArgumentParser..."
g++ -std=c++20 -c src/ArgumentParser.cpp -o ArgumentParser.o -Isrc || exit 1

echo "Compiling IconProvider..."
g++ -std=c++20 -c src/IconProvider.cpp -o IconProvider.o -Isrc || exit 1

echo "Compiling FileOperations..."
g++ -std=c++20 -c src/FileOperations.cpp -o FileOperations.o -Isrc || exit 1

echo "Compiling DisplayFormatter..."
g++ -std=c++20 -c src/DisplayFormatter.cpp -o DisplayFormatter.o -Isrc || exit 1

echo "Compiling lspp..."
g++ -std=c++20 -c src/lspp.cpp -o lspp.o -Isrc || exit 1

echo "Compiling main..."
g++ -std=c++20 -c src/main.cpp -o main.o -Isrc || exit 1

echo "Linking..."
g++ -std=c++20 *.o -o ls++ || exit 1

echo "Success! Binary created."
ls -la ls++
