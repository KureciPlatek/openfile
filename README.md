# openpict tool

openpict is a command-line tool that recursively find a type of document from the current folder where you execute the command to every sub-directory
When found all of the document (if there are) it will launch the "open" terminal command that will open all of them on a single program instance 

I build this program because I don't like waste time searching all pictures in the subirectories.
I don't like to use the picture management software.
Call me grumpy.

## Includes
* The single source file main.cpp
* The Netbeans project folder

## Setup
Run the Makefile
$ make
Copy the executable binary file in your /usr/bin/ directory
$ sudo cp openfile /usr/bin/

## Example
The folder /your/path/to/your/pictures/ contain a lot of subfolders which contains pictures in JPG files.

$ cd /your/path/to/your/pictures/
$ openfile JPG

TADAAA!!! All .JPG files are in one instance of the "Preview" program.
