# 1.4.1

- Camera zoom is accounted for when calculating gamemode bounds
- Added a warning popup when starting the generator in platformer mode


# 1.4.0

- Improved wave gameplay:
    - Decreased probability of straight line sections
    - Decreased penalty for flying away from center screen
- Added camera free mode setting
    - It's kinda bad with ship and wave
- Fixed an issue where objects get placed while the playtest is paused


# 1.3.0

- Added a toggle for every object
- Added the option to disable the spike boundary entirely
- Added a failsafe so the player doesn't die by jumping into the floor in cube/robot with 'use player clicks' enabled
- Fixed a bug where the layout would not stop generating on Mac


# 1.2.2

- Fixed jump indicators not being placed (again)
- Fixed a bug where the player would hit black and spider rings late in robot
- Fixed a bug where objects would not spawn when clicking with 'use player clicks' enabled
- Fixed spider rings creating impossible sections when flying with 'use player clicks' enabled
- Tweaked spike boundary generation:
    - A spike margin of 0 now accurately reflects the tightest possible boundary
    - Default spike margin set to 50


# 1.2.1

- Fixed missing jump indicators in cube/ball/robot/spider
- Fixed orb spam when holding with 'use player clicks' enabled
- Maybe fixed bad_alloc crash
- Tweaked spike boundary generation:
    - No upper spike boundary in cube/robot
    - Places fewer spikes in spike columns
    - Prevents the player from escaping the spike boundary with spider objects


# 1.2.0

- Improved ship and wave gameplay
- Added setting for spike margin
- Added settings for jump pad and ring generation
- Added experimental gameplay objects
- Fixed crash when placing jump indicators in cube/ball/robot/spider


# 1.1.0

- Added more settings
- Implement Geode settings menu which saves across sessions
- Updated button sprites
- Enabled editor preview when placing speed portals
- Fixed object spam when using a song offset


# 1.0.4

- Fixed an issue where too many objects would be placed at >60 FPS
- The war on crashes continues


# 1.0.3

- Maybe fixed random crashes
- Fixed some logic issues causing the player to die during generation


# 1.0.2

- Fixed display issue with BPM


# 1.0.1

- Tweaks to conform to Geode standards


# 1.0.0

- Initial release
