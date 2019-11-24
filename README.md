# joker
Automatic application launcher inspired by Plan9 Plumber

* This program receive a string as input via stdin and does something based on patterns setted in it's configuration file.
By a pattern I mean: an file extension, an url, a domain, a filename, a regex.


* I wrote this to be used in text editors, such as ed, vim, etc.  The idea is to launch
an program or cause something to happen based on the contents of a given line, like
in Acme editor.  You can, for example in vim, do:<br />
:10w !joker, to execute a custom action based on the line 10.  You can set a keybind
to make it easier and faster: map <F2> :.w !joker<CR>.  Now you just need to press
F2 to execute it.


* The configuration file is ~/.joker


You can have in ~/.joker, for example:<br />
ext=pdf<br />
command=zathura -P %num %arg

then, if you invoke the program as:<br />
echo /path/to/file.pdf:10 | joker

it will open the /path/to/file.pdf on line 10, inside the zathura document reader.


* It uses the POSIX regular expressions in Extended mode.

* The order of evaluation is: regex, url, domains, filenames, paths, extensions.  It
means that if you have a url and a regex that both matches the input given, the regex
will be executed, and so on.


Requirements and limitations:
* When passing a url as input, it needs to end in / for the domain matching.
* %num, %sec and %arg can't be used as part of commands since they are used as placeholders
for line number, manual section and user input.
* When passing a file path as input, it needs to end in /.
* The shell needs to have LANG set.


* Configuration file:
See joker.example.

* It does not have any dependencies.
* Tested on gcc 8.3.0, ARM.
