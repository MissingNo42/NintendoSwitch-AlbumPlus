# Album+ for Nintendo Switch
![Album+](/icon.jpg?w=32&h=32)
An easy to use Album dumper homebrew for Nintendo Switch.


## Install
Just put the `.nro` file (built or downloaded in release) in the `sdcard://switch/` folder and start Homebrew Launcher!


## Usage
On the main menu, just select what you want to dump to `sdcard://switch/Album`, screenshots, movies or both (from both sdcard and internal NAND storage) and press `A`.
Screenshots in `sdcard://switch/Album` will be named like this:
```
yyyy-mm-dd-HHMMSS-II-<TitleID>.jpg
```

And Movies like this:
```
yyyy-mm-dd-HHMMSS-II-<TitleID>.mp4
```
Where:
- `yyyy` is the year, 
- `mm` is the month,
- `dd` is the day, 
- `HHMMSS` is the hour, minutes and seconds,
- `II` is the ID of the file, used to distinguish files with the same date and TitleID,
- `<TitleID>` is the TitleID of game which the screenshot/movie come from: `0100XXXXXXXXXXXX`
![Menu](/.screenshots/menu.jpg)

On the dump screen, you can see the number of dumped files, the number of files to dump, the filename of the file which is currently being dumped and the elapsed time.
You can interrupt the dumping process by pressing `B` (can take some time). Do not interrupt the dumping process by any other way, like force closing the Homebrew, it may corrupt the entire sdcard (it never happened and maybe won't but be careful).

![Dump](/.screenshots/dump.jpg)


## Build
Clone the repo using `git` and just execute the following command on Windows (may work on Linux too) to build the `.nro` file (DevkitPro/DevKitA64 environment is required):
```bash
make
```

## Disclamer
I created this Homebrew out of curiosity to discover what Nintendo Switch development looks like, this is why this Homebrew is not very useful as you can just copy the official folder `sdcard://Nintendo/Album` (but files in it are uglily named and stored, and files in internal NAND storage are not here). There is not any warranty of any kind, feel free to use it at your own risks, like any other Homebrew.

Successfully tested on Switch version `14.1.2` and Atmosphère `1.3.2` ( 2022/09 ), may works perfectly below and above.


## MIT License

Copyright (c) 2022 MissingNo42

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
