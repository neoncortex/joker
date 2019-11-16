# joker
Automatic application launcher inspired by Plan9 Plumber

This program receive a string as input via stdin and does something based on patterns setted in it's configuration file.
By a pattern I mean: an file extension, an url, a domain, a filename, a regex.


I wrote this to be used in text editors, such as ed, vim, etc.  Then you can do:<br />
10w !joker

to execute a custom action based on the line.


The configuration file is ~/.joker


You can have in ~/.joker, for example:<br />
ext=pdf<br />
command=zathura -P %line

then, if you invoke the program as:<br />
echo /path/to/file.pdf:10 | joker

it will open the /path/to/file.pdf on line 10.


Limitations:
* When passing a url as input, it needs to end in / for the domain matching.
* %line can't be used as part of commands since it's used by the program as a placeholder for the line number passed as :num.
* It needs a whitespece after %line.
* When passing a file path as input, it needs to end in /.
* The shell needs to have LANG set.


See joker.example.

* It does not have any dependencies, but it uses regex.h, which I presume all unices have.
* Tested on gcc 8.3.0, ARM.
