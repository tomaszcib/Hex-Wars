# Hex Wars
![version 1.0](https://img.shields.io/badge/version-1.0-blue.svg) ![Qt 5.7+](https://img.shields.io/badge/Qt-5.7%2B-brightgreen.svg)

Hex Wars is a turn-based strategy game for 2 to 6 human or computer players where the object is to capture all your enemies' cities and shatter their forces. One must use various types of units, explore the world and discover new lands to achieve this goal, whilst procedural random world generator makes each game more in-depth and challenging.

## Downloading the game
See [release](https://github.com/tomaszcib/Hex-Wars/releases) directory for binaries and other download files.

## Building from source
The repo contains all the needed source, together with `.pro` project file. __Note:__ `SDL` and `SDL_mixer` libraries are required if you want to use audio within the game. Make sure if you have them installed.

I have used the following software for compilation:
- `Qt`: 5.7
- `g++`: 4.8.7

Although the program should be backwards-compatible with older Qt5 releases, I'd heavily recommend using Qt 5.7 or, if possible, newer version. Qt4 is not supported, as it uses different headers and some of the functions had become obsolete in the Qt5 (a guide on conversion to Qt4 is on its way).

### Additional modules
`audio` and `doc` directories have to be copied into the build destination directory.
