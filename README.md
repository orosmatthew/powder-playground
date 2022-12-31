# Powder Playground

![Game Demo gif](https://raw.githubusercontent.com/orosmatthew/powder-playground/master/img/demo.gif)

## Description

Powder Playground is a simple grid-based particle simulation inspired by [The Powder Toy](https://powdertoy.co.uk/)

## How to Play

Use the numbers keys (1-8) to select an element. Left-click to spawn element and right-click to delete.

## Building Instructions

CMake is required

```bash
git clone https://github.com/orosmatthew/powder-playground
cd powder-playground
mkdir build
cmake -S . -B build
cmake --build build
```

The executable will be in the `build/` directory but will be different depending on the generator CMake uses.

> NOTE: You must copy the `res/` resources directory into the same directory as the executable, otherwise the game will
> not be able to load the assets!