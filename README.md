Well I have addapted the project hack-alienfx to the new Alienware 13.

I remaped all the lights to this computer

And the last but no less important thing is that I write a C program that takes a file as an input and changes the keyboard lights to the lights specified in the file

# hack-alienfx
hacking AlienFX... (under heavy development)


# Usage
The device id should be changed according to the interface of your PC.
First you have to check the your Alienware Corporation device by "lsusb" and give the numbers to the source files.

Then, 
$ make all
$ ./run seq/snooze
to build and test it.

To register the command, please try the following instruction.
$ cp run /usr/local/bin/
$ mkdir /usr/local/fx
$ cp seq/* /usr/local/fx
$ chmod 4755 /bin/fx
$ cp lights.sh /usr/local/bin/lights
$ chmod +x /usr/local/bin/lights

Now, you can run
$ lights
$ lights on
$ lights off

It may useful to registering the application as startup application.


# reference
https://wiki.archlinuxjp.org/index.php/Alienware_13
