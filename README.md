# Ski-Jump-GB
A simple ski jumping game for the GameBoy programmed in C using GBDK.

# Screenshots
![Splash Screen](https://raw.githubusercontent.com/periparus/Ski-Jump-GB/main/screenshots/bgb00001.bmp)
![Ramp](https://raw.githubusercontent.com/periparus/Ski-Jump-GB/main/screenshots/bgb00002.bmp)
![In the Air](https://raw.githubusercontent.com/periparus/Ski-Jump-GB/main/screenshots/bgb00003.bmp)
![Scoreboard](https://raw.githubusercontent.com/periparus/Ski-Jump-GB/main/screenshots/bgb00004.bmp)


# Controls
* Press START to start the game after booting it up.
* The game itself is controlled entirely with the A button.
* Press A to launch off the ramp
* Press A again to jump
* After landing, press A to view the scoreboard
* Press A to advance through the scoreboard
* Press START to restart the game

# Compiling the Source
If you want to compile the source for yourself, all you need are the files with the `.c` extension (in th same directory). The Graphics folder and other files are for use with GBTD / GBMB or for convenience. To build the project, simply run the `make.bat` and the project should build into a `.gb` file, along with a couple other files.

# Thank Yous
Thank you to [GamingMonsters](https://github.com/gingemonster) and his [YouTube channel](https://www.youtube.com/channel/UCMMhSfBStEti-Lqzs30HYWw); the videos were an excellent introduction to GameBoy programming and helped me tremendously to grasp all the GBDK specific concepts.  
Tiles and Maps were created with the amazing tools [GBTD](http://devrs.com/gb/hmgd/gbtd.html) and [GBMB](http://devrs.com/gb/hmgd/gbmb.html), I can't imagine working without them.  
Zal0's [GBSoundDemo](https://github.com/Zal0/GBSoundDemo) made creating sound effects very simple and I'm grateful for that, because I'm no good with imagining sounds in bits in registers!
