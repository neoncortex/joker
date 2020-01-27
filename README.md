# joker
Automatic application launcher inspired by Plan9 Plumber

* This program receive a string as input via stdin and does something based on patterns setted in it's configuration file.
It can deal with: file extensions, url's, domains, filenames, regex, manpages, C headers.


* I wrote this to be used in text editors, such as ed, vim, etc.  The idea is to launch
an program or cause something to happen based on the contents of a given string, like
in Acme.  It's useful to bookmark things:
* /path/to/file.pdf:10  # page 10 of a pdf file.
* printf(3):30  # line 30 of the section 3 printf manpage.
* <stdio.h>:25  # line 25 of the C header stdio.h.
* /path/to/file.mp4:60  # video file, which will start at second 60
* http://websitex.com  # hyperlink


The : is used to point to a page in document, time in a video/audio file, line number in a file, and if you are using nvim as in the joker.example, it can be used to pass a pattern, like: <stdio.h>:/#define.  This will place the cursor in the first #define ocurrence after opening the file.  You can pass any nvim command since it's using the + (see nvim 0.3.4: nvim(1):129, nvim(1):134, nvim(1):141).  You can use that to create sort of links in files, you just need unique text pattern.


For nvim, I added to the init.vim:<br />
map <F2> :.w !joker<CR><br />
vmap <F2> "1y:!echo '\<C-R\>1'\|joker<CR><br />


I suppose it will be the same for vim.  After that, you use F2 key to invoke joker.
  If F2 is pressed on command mode, it sends the current line content to joker.
  It's useful if your document is formatted for these use in mind and the line have
a meaningful content for joker.  If you press F2 in visual mode, it sends the actual
selection to joker.  It will use register 1 of the (n)vim editor, so any data there
will be erased.


You can send multiple consecutive lines easily, for example: :10,20w !joker


* The configuration file is ~/.joker


You can have in ~/.joker, for example:<br />
ext=pdf<br />
command=zathura -P %num %arg

then, if you invoke the program as:<br />
echo /path/to/file.pdf:10 | joker<br />
or press F2 when a line/selection contains the /path/to/pdf:10

it will open the /path/to/file.pdf on page 10, inside the zathura document reader.


* It uses the POSIX regular expressions in Extended mode.

* The order of evaluation is: regex, url, domains, filenames, paths, extensions, manpages.  It
means that if you have a url and a regex that both matches the input given, the regex
will be executed, and so on.


Requirements and limitations:
* When passing a url as input, it needs to end in / for the domain matching.
* %num, %sec and %arg can't be used as part of commands since they are used as placeholders.
* The : character can't be used in filenames, since it's used as a placeholder for line/pattern.
* When passing a directory as input, it's path needs to end in /.
* Relative path's must start with ./ (unless it points a directory up ../).
* If it receives a "string" like that, it will be interpreted as a relative path, will be appended ./ and dealt with as a file.
* The shell needs to have LANG set.


Configuration file:
* See joker.example.


Build:
* make.


And:
* It does not have any dependencies.
* Tested on gcc 8.3.0, ARM.
