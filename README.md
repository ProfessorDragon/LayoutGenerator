# Layout Generator

Procedurally generate Geometry Dash layouts.


## Creating a layout

1. Create a new level and open the editor.
2. If the mod is installed correctly, there will be new buttons in the top left corner, next to the music playback button.
3. Click on the BPM button and enter the BPM of the song you're using.
4. Click the Layout Generator button to start generating.
5. It will go for as long as you want. Click the stop button to stop generating.


## Build instructions

For more info, see the [Geode docs](https://docs.geode-sdk.org/getting-started/create-mod#build)
```sh
# Assuming you have the Geode CLI set up already
geode build
```


## Known issues

- Jump indicators in cube/ball/robot/spider may be missing when the jumps are buffered. (If anyone has ideas on how to fix this, lmk!)
- The player can escape the spike boundary by tapping a spider ring in cube/ball/robot late.
- The player can die in wave by hitting a gamemode portal when sandwiched between the level bounds and another block.
