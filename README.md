# joker
Automatic application launcher inspired by Plan9 Plumber

This program receive a string as input via stdin and does something based on patterns setted in it's configuration file.
By a pattern I mean: an file extension, an url, a domain, a filename, a regex.


I wrote this to be used in text editors, such as ed, vim, etc.  Then you can do:
10w !joker

to execute a custom action based on the line.


The configuration file is ~/.joker


You can have in ~/.joker, for example:
ext=pdf
command=zathura -P %line

then, if you invoke the program as:
echo /path/to/file.pdf:10

it will open the /path/to/file.pdf on line 10.


Limitations:
* When passing a url as input, it needs to end in / for the domain matching.
* %line can't be used as part of commands since it's used by the program as a placeholder for the line number passed as :num.
* It needs a whitespece after %line.
* When passing a file path as input, it needs to end in /.


See joker.example.
