# TV Namer

_A program to automatically rename your TV video files using the The TVDB._

## Building and Running

    $ mkdir build && cd build
    $ cmake ..
    $ make
    $ ./src/tvnamer

## Usage

The program is very simple to use. Once you have opened the program, you should press "Auto Find Shows" and point it to your directory where all your TV shows are stored. If you would rather do this manually, you can click "Find Show" for each show you own.

The column on the left will be populated with all the shows and seasons. When you click on a season, the column on the right will show all the file renames that will occur when you press "Save Changes". You are free to change these by double clicking on an item. Leaving any item blank will stop any renaming for that episode.

The items are colour coded to make things clearer. Green means the filenames are the same so there will be no change. Orange means the filenames will be changed and the program thinks it has a pretty good match. Red means the program did not get a match, so you should provide one.

## Guarantee

This program makes no guarantees. I've been testing it live with my TV collection and I haven't had any problems, but I cannot guarantee that it will never get things wrong.

Before you click "Save Changes" *please* ensure that you have checked all the red and orange items and are happy that they are correct.
