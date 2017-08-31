/****************************************************************************
**
**
**    Hex Wars - Copyright (C) 2017 Tomasz Ciborski
**
**    This file is part of Hex Wars.
**
**    Hex Wars is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    Hex Wars is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with Hex Wars.  If not, see <http://www.gnu.org/licenses/>.
**
**
****************************************************************************/

/****************************************************************************
 *
 *  /ui/strings.cpp
 *
 *  Strings class constructor. Related to /ui/strings.h header file.
 *
 * *************************************************************************/

#include "strings.h"

Strings::Strings(){
    string[0] = "<b>Minimap</b>";
    string[1] = "<b>Selection</b>";
    string[2] = "HP:\t";
    string[3] = "MP:\t";
    string[4] = "Fuel:\t";
    string[5] = "<b>Cities controlled</b>";
    string[6] = "Previous unit";
    string[7] = "Next unit";
    string[8] = "Deselect";
    string[9] = "Land paratroopers";
    string[10] = "Focus on unit";
    string[11] = "Disband unit";
    string[12] = "Inspect city";
    string[13] = "End turn";

    string[14] = " in ";
    string[15] = " turn(s)";

    string[16] = "Now playing...";

    string[17] = "Tank";
    string[18] = "Paras";
    string[19] = "Fighter";
    string[20] = "Bomber";
    string[21] = "Transport";
    string[22] = "Destroyer";
    string[23] = "Battleship";
    string[24] = "Carrier";
    string[25] = "Submarine";

    string[26] = "City production";
    string[27] = "City Close-Up";

    string[28] = "New Game";
    string[29] = "World generation";
    string[30] = "Players";
    string[31] = "Seed";
    string[32] = "World size";
    string[33] = "Amount of land";
    string[34] = "Player positions";
    string[35] = "Small";
    string[36] = "Medium";
    string[37] = "Large";
    string[38] = "Random";
    string[39] = "Fixed";
    string[40] = "Not in game";
    string[41] = "Human";
    string[42] = "AI Easy";
    string[43] = "AI Medium";
    string[44] = "AI Hard";

    string[45] = "Edit piece rules";
    string[46] = "Rules...";
    string[47] = "Production\n(1-32)";
    string[48] = "Moves\n(1-32)";
    string[49] = "Fuel\n(1-4)";

    string[50] = "OK";
    string[51] = "Cancel";

    string[52] = "File";
    string[53] = "Help";
    string[54] = "New game";
    string[55] = "Load game";
    string[56] = "Save game";
    string[57] = "Exit";
    string[58] = "About...";
    string[59] = "Website";
    string[60] = "Game manual";
    string[61] = "Save files (*.sav)";

    string[62] = "has won the game!";
    string[63] = "There are no human players left in the game!";
    string[64] = "End of the game";
    string[65] = "has been defeated!";
    string[66] = "Player eliminated";

    string[67] = "Hex Wars - information";
    string[68] = "<font size=4><b>Hex Wars</b></font><br>version 1.0<br><br>A strategy game of exploration, expansion and world conquest.\
            <br><br><b>Author:</b>&#09;Tomasz Ciborski (2017)<br>Licensed under GNU General Public License v3";

    string[69] = "Rankings";
    string[70] = "High scores";
    string[71] = "spectator";
    string[72] = "Default";

    string[73] = "Error";
    string[74] = "Could not load game from file ";
    string[75] = "Could not save game into file ";
    string[76] = "Empty file name";
    string[77] = "No file name specified.";
    string[78] = "Save game exists";
    string[79] = " already exists. Do you want to overwrite it?";
    string[80] = "Surrender";
    string[81] = "Do you really want to surrender?";

    string[82] = "1st";
    string[83] = "2nd";
    string[84] = "3rd";
    string[85] = "4th";
    string[86] = "5th";
    string[87] = "6th";

    string[88] = "Player";
    string[89] = "Cities";
    string[90] = "Units";
    string[91] = "Score";

    string[92] = "world";
    string[93] = "turn";

    string[94] = "Tools";
    string[95] = "Game rankings";
    string[96] = "High scores";
    string[97] = "Program log";
    string[98] = "Difficulty";
    string[99] = "Multiplayer";
    string[100] = "No human players!";
    string[101] = "Only one player!";

    string[102] = "Incompatible version";
    string[103] = "The file is not valid. It may have come from different version of the game or been corrupted.";

    string[104] = "Congratulations!";
    string[105] = "You've scored good enough to be placed in the Highscores table!\nPlease enter your name:";

    string[106] = "Could not load high scores. Check if the program has permission to read and write in folder ";

    string[107] = "Name";
    string[108] = "Seed";
    string[109] = "Size";
    string[110] = "Land";
    string[111] = "Difficulty";
    string[112] = "Turns";
    string[113] = "Score";

    string[114] = "GAME";
    string[115] = "INFO";
    string[116] = "WARN";
    string[117] = "ERROR";
    string[118] = "DEBUG";

    string[119] = "Exit program";
    string[120] = "Do you really want to exit the game?";

    string[121] = "Do you really want to disband this unit?";

    string[122] = "Go to hex";
    string[123] = "Row";
    string[124] = "Column";

    string[125] = "In-game sounds";
    string[126] = "Show grid";
    string[127] = "Prompt on human";
    string[128] = "Options";

    string[129] = "'s turn.";

    log[0] = "Init program";
    log[1] = "Load highscores";
    log[2] = "No 'scores.dat'. Creating one";
    log[3] = "Loading game";
    log[4] = "Loading complete";
    log[5] = "Saving game";
    log[6] = "Saving complete";
    log[7] = "Starting new game";
    log[8] = "Creating terrain...";
    log[9] = "Creating islands...";
    log[10] = "Creating regions...";
    log[11] = "Spawning cities...";
    log[12] = "Spawning players...";
    log[13] = "New game OK";
    log[14] = "Red";
    log[15] = "Green";
    log[16] = "Blue";
    log[17] = "Yellow";
    log[18] = "Cyan";
    log[19] = "Magenta";
    log[20] = " has been defeated";
    log[21] = "Game ended. ";
    log[22] = "Switching view to spectator";
    log[23] = "Exiting program";
    log[24] = "Load options";
    log[25] = "No 'hexwars.dat'. Using default settings";

}
