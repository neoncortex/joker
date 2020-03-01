#include "joker.h"

struct container *fext, *links, *domain, *filename, *regex, *wsearch;
wchar_t *filemanager, *stdaction, *browser, *manpage, *history;
struct wlist *idir, *argl;

wchar_t*
wscopy(wchar_t *com)
{
	if(com == NULL)
		return NULL;

	wchar_t *tmp = malloc((wcslen(com) + 1)
		* sizeof(wchar_t));
	if(tmp == NULL)
		return NULL;

	wchar_t *str = wcscpy(tmp, com);
	if(str[wcslen(str) - 1] == L'\n')
		str[wcslen(str) - 1] = L'\0';

	return str;
}

struct container*
adddata(struct container *cont, wchar_t *tail)
{
	if(cont == NULL || tail == NULL)
		return NULL;

	struct data *d = datanew();
	if(d == NULL)
		return NULL;

	d->desc = wscopy(tail);
	int res = containerinsert(cont, d);
	if(res != 0)
		wprintf(L"problem containerinsert\n");

	return cont;
}

struct data*
selector(int x)
{
	struct data *d = NULL;
	if(x == 1)
		d = fext->d[fext->size - 2];
	else if(x == 2)
		d = links->d[links->size - 2];
	else if(x == 3)
		d = domain->d[domain->size - 2];
	else if(x == 4)
		d = filename->d[filename->size - 2];
	else if(x == 5)
		d = regex->d[regex->size - 2];
	else if(x == 6)
		d = wsearch->d[wsearch->size - 2];

	return d;
}

int
readconfig(char *file)
{
	if(file == NULL)
		return -1;

	FILE *fd = fopen(file, "r");
	if(fd == NULL) {
		perror("Error readconfig > fopen()");
		return -2;
	}

	int size = 2;
	int error = 0;
	wchar_t c = 0;
	wchar_t *h = NULL;
	wchar_t *t = NULL;
	wchar_t *line = malloc(size * sizeof(wchar_t));
	if(line == NULL) {
		fclose(fd);
		return -1;
	}

	int i = 0, j = 0;
	int addflag = 0;
	while((c = fgetwc(fd)) != WEOF) {
		line[size - 2] = c;
		void *ret = realloc(line, (size + 1) * sizeof(wchar_t));
		if(ret == NULL) {
			fclose(fd);
			error = 1;
			goto cleanline;
		}

		line = ret;
		size++;
		line[size - 1] = L'\0';
		if(c != L'\n')
			continue;

		if((line[0] == L'#')
		|| (line[0] == L'\n')
		|| (line[0] == L' ')
		|| (line[0] == L'\t'))
			goto cleanline;

		int pos = 0;
		for(i = 0; i < size - 1; ++i) {
			if(line[i] == L'=') {
				pos = i;
				break;
			}
		}

		if(pos == 0)
			goto cleanline;

		h = malloc(size * sizeof(wchar_t));
		if(h == NULL) {
			error = 1;
			goto cleanline;
		}

		t = malloc(size * sizeof(wchar_t));
		if(t == NULL) {
			error = 1;
			goto cleanline;
		}

		for(i = 0; i < pos; ++i)
			h[i] = line[i];

		j = 0;
		for(i = pos + 1; i < size - 1; ++i) {
			t[j] = line[i];
			j++;
		}

		h[pos] = L'\0';
		t[size - 1] = L'\0';
		if(wcscmp(h, L"ext") == 0) {
			struct container *temp = adddata(fext, t);
			if(temp == NULL)
				addflag = 0;
			else {
				fext = temp;
				addflag = 1;
			}
		} else if(wcscmp(h, L"url") == 0) {
			struct container *temp = adddata(links, t);
			if(temp == NULL)
				addflag = 0;
			else {
				links = temp;
				addflag = 2;
			}
		} else if(wcscmp(h, L"domain") == 0) {
			struct container *temp = adddata(domain, t);
			if(temp == NULL)
				addflag = 0;
			else {
				domain = temp;
				addflag = 3;
			}
		} else if(wcscmp(h, L"filename") == 0) {
			struct container *temp = adddata(filename, t);
			if(temp == NULL)
				addflag = 0;
			else {
				filename = temp;
				addflag = 4;
			}
		} else if(wcscmp(h, L"regex") == 0) {
			struct container *temp = adddata(regex, t);
			if(temp == NULL)
				addflag = 0;
			else {
				regex = temp;
				addflag = 5;
			}
		} else if(wcscmp(h, L"searchengine") == 0) {
			struct container *temp = adddata(wsearch, t);
			if(temp == NULL)
				addflag = 0;
			else {
				wsearch = temp;
				addflag = 6;
			}
		} else if(wcscmp(h, L"filemanager") == 0) {
			if(filemanager != NULL)
				free(filemanager);

			filemanager = wscopy(t);
			addflag = 0;
		} else if(wcscmp(h, L"stdaction") == 0) {
			if(stdaction != NULL)
				free(stdaction);

			stdaction = wscopy(t);
			addflag = 0;
		} else if(wcscmp(h, L"browser") == 0) {
			if(browser != NULL)
				free(browser);

			browser = wscopy(t);
			addflag = 0;
		} else if(wcscmp(h, L"manpage") == 0) {
			if(manpage != NULL)
				free(manpage);

			manpage = wscopy(t);
			addflag = 0;
		} else if(wcscmp(h, L"history") == 0) {
			if(history != NULL)
				free(history);

			history = wscopy(t);
			addflag = 0;
		} else if(wcscmp(h, L"idir") == 0) {
			if((wlistinsert(idir, (wscopy(t)))) != 0)
				wprintf(L"problem wlistinsert\n");
			addflag = 0;
		} else if(wcscmp(h, L"include") == 0) {
			char *cf = calloc((wcslen(t)), sizeof(char));
			if(cf == NULL) {
				error = 1;
				goto cleanline;
			}

			if(t[wcslen(t) - 1] == L'\n')
				t[wcslen(t) - 1] = L'\0';

			if((wcstombs(cf, t, wcslen(t))) > 0)
				readconfig(cf);

			free(cf);
			addflag = 0;
		} else if(wcscmp(h, L"command") == 0) {
			struct data *d = selector(addflag);
			if(d != NULL) {
				if((datainsert(d, (wscopy(t)), 2)) != 0)
					wprintf(L"problem datainsert\n");
			}
		} else {
			wprintf(L"unknown parameter: %ls\n", h);
			struct data *d = selector(addflag);
			if(d != NULL)
				d->exec = NULL;
		}

cleanline:
		free(h);
		free(t);
		free(line);
		t = h = NULL;
		if(error == 1) {
			fclose(fd);
			return -1;
		}

		size = 2;
		line = malloc(size * sizeof(wchar_t));
		if(line == NULL) {
			fclose(fd);
			return -1;
		}
	}

	if(line != NULL)
		free(line);

	fclose(fd);
	return 0;
}

int
extmatch(wchar_t *arg, wchar_t *ext)
{
	if(arg == NULL || ext == NULL)
		return -1;

	int i;
	int esize = wcslen(ext);
	int asize = wcslen(arg);
	for(i = 0; i < esize; ++i) {
		if((arg[asize - i - 1] == L'.')
		&& (esize - i == 0))
			break;

		if(arg[asize - i - 1] != ext[esize - i - 1])
			return 0;
	}

	return 1;
}

wchar_t*
replace(wchar_t *str, wchar_t *placeholder, wchar_t *data
	,wchar_t *format, int extrasize)
{
	if(str == NULL || data == NULL || placeholder == NULL
	|| format == NULL)
		return NULL;

	int i, j, k, size = 0;
	int csize = wcslen(str);
	wchar_t *head = NULL, *tail = NULL;
	for(i = 0; i < csize; ++i) {
		if((str[i] == L'%')
		&& (i + 3 <= csize)
		&& (str[i + 1] == placeholder[0])
		&& (str[i + 2] == placeholder[1])
		&& (str[i + 3] == placeholder[2])) {
			head = malloc((csize)
				* sizeof(wchar_t));
			if(head == NULL)
				break;

			for(j = 0; j < i; ++j)
				head[j] = str[j];

			head[j] = L'\0';
			tail = malloc(csize
				* sizeof(wchar_t));
			if(tail == NULL) {
				free(head);
				break;
			}

			k = 0;
			for(j = i + 4; j < csize; ++j) {
				tail[k] = str[j];
				++k;
			}

			tail[k] = L'\0';
			size = wcslen(head)
				+ wcslen(data)
				+ wcslen(tail)
				+ extrasize;
			wchar_t *cmd = malloc(size
				* sizeof(wchar_t));
			if(cmd == NULL) {
				free(head);
				free(tail);
				break;
			}

			swprintf(cmd, size, format
				,head
				,data
				,tail);
			free(head);
			free(tail);
			return cmd;
		}
	}

	return NULL;
}

void
evaluate(wchar_t *arg, FILE *hist)
{
	if(arg == NULL)
		return;

	if(hist != NULL) {
		fwrite(arg, wcslen(arg) * sizeof(wchar_t), 1, hist); 
		fwrite("\n", sizeof(char), 1, hist); 
	}

	unsigned int i, j, k, csize, freearg;
	i = j = k = csize = freearg = 0;
	unsigned int asize = wcslen(arg);
	wchar_t *linenum = NULL;
	wchar_t *command = NULL;
	wchar_t *cmd = NULL;
	wchar_t *tmp = NULL;
	for(i = 0; i < asize; ++i) {
		if((arg[i] == L':')
		&& ((i + 1) < asize)
		&& (arg[i + 2] != L'/')) {
			j = asize - i;
			linenum = malloc(asize * sizeof(wchar_t));
			if(linenum == NULL)
				break;

			for(k = 0; k < j; ++k)
				linenum[k] = arg[i + k + 1];

			linenum[wcslen(linenum)] = L'\0';
			wchar_t *carg = malloc(asize
				* sizeof(wchar_t));
			if(carg == NULL)
				break;

			for(k = 0; arg[k] != L':'; ++k)
				carg[k] = arg[k];

			carg[k] = L'\0';
			arg = carg;
			asize = wcslen(arg);
			freearg = 1;
			break;
		}
	}

	if(linenum == NULL) {
		linenum = malloc(2 * sizeof(wchar_t));
		if(linenum == NULL)
			goto evaluation;

		linenum[0] = L'1';
		linenum[1] = L'\0';
	}

	for(i = 0; i < regex->size - 1; ++i) {
		struct data *d = regex->d[i];
		if(d == NULL)
			break;

		int ret;
		char *pattern = calloc(wcslen(d->desc) + 2
			,sizeof(char));
		if(pattern == NULL)
			break;

		if((wcstombs(pattern, d->desc, wcslen(d->desc) + 1)) <= 0) {
			free(pattern);
			continue;
		}

		regex_t reg;
		ret = regcomp(&reg, pattern, REG_EXTENDED);
		free(pattern);
		if(ret != 0) {
			wprintf(L"Error regcomp(): %ld\n", ret);
			regfree(&reg);
			continue;
		}

		char *carg = calloc(wcslen(arg) + 2, sizeof(char));
		if(carg == NULL) {
			regfree(&reg);
			break;
		}

		if((wcstombs(carg, arg, wcslen(arg) + 1)) <= 0) {
			regfree(&reg);
			free(carg);
			break;
		}

		ret = regexec(&reg, carg, 0, NULL, 0);
		free(carg);
		regfree(&reg);
		if(ret != 0)
			continue;

		command = d->exec;
		goto evaluation;
	}

	if((arg[0] == L'<')
	&& (arg[asize - 1] == L'>')) {
		for(i = 0; i < idir->size - 1; ++i) {
			wchar_t *ipath = idir->list[i];
			int psize = wcslen(ipath) + wcslen(arg);
			wchar_t *p = malloc(psize * sizeof(wchar_t));
			if(p == NULL)
				break;

			wchar_t *fn = malloc(asize * sizeof(wchar_t));
			if(fn == NULL) {
				free(p);
				break;
			}

			for(j = 1; j < asize - 1; ++j)
				fn[j - 1] = arg[j];

			fn[j - 1] = '\0';
			swprintf(p, psize, L"%ls%ls", ipath, fn);
			free(fn);
			char *cp = calloc((psize + 1), sizeof(char));
			if(cp == NULL) {
				free(p);
				break;
			}

			if((wcstombs(cp, p, wcslen(p))) <= 0) {
				free(cp);
				free(p);
				break;
			}

			int ret = access(cp, F_OK);
			free(cp);
			if(ret == 0) {
				if(freearg == 1)
					free(arg);
				else
					freearg = 1;

				arg = p;
				asize = wcslen(arg);
				break;
			}
		}
	}

	if((arg[0] == L'\"')
	&& (arg[asize - 1] == L'\"')) {
		wchar_t *p = malloc((asize + 1) * sizeof(wchar_t));
		if(p == NULL)
			goto evaluation;

		p[0] = L'.';
		p[1] = L'/';
		for(i = 2; i < asize; ++i)
			p[i] = arg[i - 1];

		if(freearg == 1)
			free(arg);
		else
			freearg = 1;

		arg = p;
		asize = wcslen(arg);
	}

	if(!(wcsncmp(arg, L"http", 4))
	|| !(wcsncmp(arg, L"https", 5))) {
		for(i = 0; i < links->size - 1; ++i) {
			struct data *d = links->d[i];
			if(d == NULL)
				break;

			if(wcsncmp(arg, d->desc, wcslen(d->desc)) == 0) {
				command = d->exec;
				goto evaluation;
			}
		}

		for(i = 0; i < domain->size - 1; ++i) {
			struct data *d = domain->d[i];
			if(d == NULL)
				break;

			unsigned int begin, end;
			begin = 7;
			if(arg[begin] == L'/')
				begin++;

			end = begin;
			while((arg[end] != L'/')
			&& (end < asize))
				end++;

			wchar_t *dm = malloc(asize * sizeof(wchar_t));
			if(dm == NULL)
				break;

			j = 0;
			for(k = begin; k < end; ++k) {
				dm[j] = arg[k];
				j++;
			}

			dm[j] = L'\0';
			if(wcscmp(dm, d->desc) == 0) {
				command = d->exec;
				free(dm);
				goto evaluation;
			}

			free(dm);
		}

		command = browser;
		goto evaluation;
	}

	if((arg[0] == L'/')
	|| (arg[0] == L'.' && arg[1] == L'/')
	|| (arg[0] == L'.' && arg[1] == L'.' && arg[2] == L'/')) {
		for(i = 0; i < filename->size - 1; ++i) {
			struct data *d = filename->d[i];
			if(d == NULL)
				break;

			if(extmatch(arg, d->desc) == 1) {
				command = d->exec;
				goto evaluation;
			}
		}

		if(arg[asize - 1] == L'/') {
			command = filemanager;
			goto evaluation;
		}

		int havext = 0;
		for(i = asize; i > 0; --i) {
			if(arg[i] == L'/')
				break;

			if(arg[i] == L'.') {
				havext = 1;
				break;
			}
		}

		if(havext == 0) {
			command = stdaction;
			goto evaluation;
		}

		for(i = 0; i < fext->size - 1; ++i) {
			struct data *d = fext->d[i];
			if(d == NULL)
				break;

			if(extmatch(arg, d->desc) == 1) {
				command = d->exec;
				goto evaluation;
			}
		}

		command = stdaction;
		goto evaluation;
	}

	if((arg[asize - 3] == L'(')
	&& (arg[asize - 1] == L')')) {
		wchar_t *sec = malloc(2 * sizeof(wchar_t));
		if(sec == NULL) {
			command = NULL;
			goto evaluation;
		}

		sec[0] = arg[asize - 2];
		sec[1] = L'\0';
		command = manpage;
		wchar_t *tmp = replace(command, L"sec", sec, L"%ls%ls%ls"
			,1);
		free(sec);
		if(tmp == NULL) {
			free(command);
			command = NULL;
			goto evaluation;
		}

		free(cmd);
		cmd = tmp;
		command = cmd;
		wchar_t *newarg = malloc(asize * sizeof(wchar_t));
		if(newarg == NULL) {
			free(command);
			command = NULL;
			goto evaluation;
		}

		for(i = 0;; ++i) {
			if(arg[i] == L'(') {
				newarg[i] = L'\0';
				break;
			}

			newarg[i] = arg[i];
		}

		if(freearg == 1)
			free(arg);
		else
			freearg = 1;

		arg = newarg;
		asize = wcslen(arg);
	}
	
	if(asize > 7
	&& wcsncmp(arg, L"search(", 7) == 0) {
		wchar_t *engine = malloc(asize * sizeof(wchar_t));
		if(engine == NULL) {
			command = NULL;
			goto evaluation;
		}

		wchar_t *sterm = malloc(asize * sizeof(wchar_t));
		if(sterm == NULL) {
			free(engine);
			command = NULL;
			goto evaluation;
		}

		j = 0;
		for(i = 7;; ++i) {
			if(arg[i] == L',' || i == asize)
				break;

			engine[j] = arg[i];
			j++;
		}

		engine[j] = L'\0';
		j = 0;
		for(i += 1;; ++i) {
			if(arg[i] == L')' || i == asize)
				break;

			sterm[j] = arg[i];
			j++;
		}

		sterm[j] = L'\0';
		for(i = 0; i < wsearch->size - 1; ++i) {
			struct data *d = wsearch->d[i];
			if(d == NULL)
				break;

			if(d->desc == NULL || d->exec == NULL)
				continue;

			if(wcscmp(d->desc, engine) == 0) {
				if(freearg == 1)
					free(arg);
				else
					freearg = 1;
				
				arg = sterm;
				asize = wcslen(arg);
				command = d->exec;
				free(engine);
				goto evaluation;
			}
		}

		free(engine);
		free(sterm);
	}

evaluation:
	if(command != NULL) {
		tmp = replace(command, L"num", linenum, L"%ls%ls%ls", 1);
		if(tmp != NULL) {
			free(cmd);
			cmd = tmp;
			command = cmd;
		}

		tmp = replace(command, L"arg", arg, L"%ls'%ls'%ls", 3);
		if(tmp != NULL) {
			free(cmd);
			cmd = tmp;
			command = cmd;
		}

		char *scommand = malloc((wcslen(command) + 2)
			* sizeof(char));
		if(scommand != NULL) {
			int ret = wcstombs(scommand, command, (wcslen(command)
				+ 2));
			if(ret > 0)
				system(scommand);

			free(scommand);
		}
	}

	free(linenum);
	free(cmd);
	if(freearg == 1)
		free(arg);
}

int
dataread(FILE *fp)
{
	if(fp == NULL)
		return -1;

	wchar_t c;
	while((c = fgetwc(fp)) != WEOF) {
		int size = 2;
		wchar_t *arg = malloc(size * sizeof(wchar_t));
		if(arg == NULL)
			return -1;

		if(c == L'\n') {
			free(arg);
			continue;
		}

		do {
			if((size == 2 && c == L' ')
			|| (size == 2 && c == L'\t'))
				continue;

			arg[size - 2] = c;
			void *ret = realloc(arg, (size + 1)
				* sizeof(wchar_t));
			if(ret == NULL) {
				free(arg);
				return -1;
			}

			size++;
			arg = ret;
			arg[size - 2] = L'\0';
		} while((c = fgetwc(fp)) != L'\n');

		if(size > 2) {
			if((wlistinsert(argl, arg)) != 0)
				wprintf(L"problem wlistinsert\n");
		} else
			free(arg);
	}

	return 0;
}

char*
stringsep(char *str, char delim)
{
	unsigned int i, j;
	unsigned int size = strlen(str);
	for(i = 0; i < size; ++i) {
		if(str[i] == delim) {
			++i;
			break;
		}
	}

	if(i == size)
		return NULL;

	char *ret = calloc(size, sizeof(char));
	if(ret == NULL)
		return NULL;

	for(j = 0; j < size - 1; ++j) {
		ret[j] = str[i];
		++i;
	}

	ret[j] = '\0';
	return ret;
}

char*
pathsolve(char *directory, char *home, char *def)
{
	if(home == NULL || def == NULL)
		return NULL;

	char *path = NULL;
	if(directory == NULL) {
		int size = strlen(home)
			+ strlen(def)
			+ 2;
		path = calloc(size, sizeof(char));
		if(path == NULL)
			return NULL;

		snprintf(path, size, "%s/%s", home, def);
	} else {
		path = calloc((strlen(directory) + 1), sizeof(char));
		if(path == NULL)
			return NULL;

		strcpy(path, directory);
	}

	return path;
}

void
help()
{
	wprintf(L"joker: automatic application launcher inspired by Plan9"
		" Plumber\n");
	wprintf(L"Usage: piped command | joker OPTIONS\n");
	wprintf(L"       joker OPTIONS str1 str2 strn\n\n");
	wprintf(L"OPTIONS\n");
	wprintf(L"-c=file: config file\n");
	wprintf(L"-f=file: use file content as input\n");
	wprintf(L"-i=file: history file\n");
	wprintf(L"-n: do not write the pattern in history file\n");
	wprintf(L"-h: help: this\n");
}

int
main(int argc, char **argv)
{
	setlocale(LC_ALL, getenv("LANG"));
	char *cname = NULL;
	char *hname = NULL;
	argl = wlistnew();
	if(argl == NULL)
		return -1;

	unsigned int reg = 1;
	int i, r = -1;
	for(i = 1; i < argc; ++i) {
		if(strcmp(argv[i], "-h") == 0) {
			help();
			return 0;
		} else if(strncmp(argv[i], "-c", 2) == 0) {
			cname = stringsep(argv[i], '=');
		} else if(strncmp(argv[i], "-i", 2) == 0) {
			hname = stringsep(argv[i], '=');
		} else if(strcmp(argv[i], "-n") == 0) {
			reg = 0;
		} else if(strncmp(argv[i], "-f", 2) == 0) {
			char *fn = stringsep(argv[i], '=');
			if(fn != NULL) {
				FILE *f = fopen(fn, "r");
				if(f == NULL) {
					perror("Error -f > fopen():");
					free(fn);
					goto clear;
				}

				int res = dataread(f);
				fclose(f);
				free(fn);
				if(res == -1)
					goto clear;
			}
		} else {
			wchar_t *t = malloc((strlen(argv[i]) + 1)
				* sizeof(wchar_t));
			if(t == NULL)
				goto clear;

			if((mbstowcs(t, argv[i], strlen(argv[i]))) <= 0) {
				free(t);
				goto clear;
			}

			if((wlistinsert(argl, t)) != 0)
				wprintf(L"problem wlistinsert\n");
		}
	}

	if(argl->size == 1) {
		if((dataread(stdin)) == -1)
			goto clear;
	}

	if(argl->size > 1) {
		char *home = NULL;
		char *cpath = NULL;
		filemanager = NULL;
		stdaction = NULL;
		browser = NULL;
		manpage = NULL;
		history = NULL;
		fext = containernew();
		links = containernew();
		domain = containernew();
		regex = containernew();
		filename = containernew();
		wsearch = containernew();
		idir = wlistnew();
		if(fext == NULL|| links == NULL|| domain == NULL
		|| regex == NULL|| filename == NULL
		|| wsearch == NULL || idir == NULL)
			goto clear;

		if((wlistinsert(idir, (wscopy(D_INCLUDE_DIR)))) != 0)
			wprintf(L"problem wlistinsert\n");

		home = getenv("HOME");
		if(home == NULL)
			goto clear;

		cpath = pathsolve(cname, home, D_CONFIG_FILE);
		if(cpath == NULL)
			goto clear;

		if((readconfig(cpath)) == 0) {
			char *histp = NULL;
			if(hname != NULL) {
				if(history != NULL)
					free(history);

				histp = pathsolve(hname, home, D_HIST_FILE);
				if(histp == NULL)
					goto clear;
				history = malloc((strlen(histp) + 1)
					* sizeof(wchar_t));
				if(history == NULL) {
					free(histp);
					goto clear;
				}

				int res = mbstowcs(history, histp
					,strlen(histp));
				if(res <= 0) {
					free(histp);
					goto clear;
				}
			} else if(history != NULL) {
				histp = malloc((wcslen(history) + 1)
					* sizeof(char));
				if(histp == NULL)
					goto clear;

				int res = wcstombs(histp, history
					,wcslen(history));
				if(res <= 0) {
					free(histp);
					goto clear;
				}
			} else
				histp = pathsolve(hname, home, D_HIST_FILE);

			FILE *fh = NULL;
			if(reg == 1) {
				fh = fopen(histp, "a");
				if(fh == NULL)
					perror("Failed to open history:");
			}

			unsigned int i;
			for(i = 0; i < argl->size - 1; ++i) {
				pid_t pid = fork();
				if(pid == 0) {
					evaluate(argl->list[i], fh);
					break;
				} else if(pid == -1)
					perror("Error fork():");
			}

			if(fh != NULL)
				fclose(fh);

			free(histp);
		} else {
			r = 1;
			goto clear;
		}

		r = 0;
clear:
		free(cpath);
		free(filemanager);
		free(stdaction);
		free(browser);
		free(manpage);
		free(history);
		containerdestroy(fext);
		containerdestroy(links);
		containerdestroy(domain);
		containerdestroy(regex);
		containerdestroy(filename);
		containerdestroy(wsearch);
		wlistdestroy(idir);
	} else
		help();

	free(cname);
	free(hname);
	wlistdestroy(argl);
	return r;
}

