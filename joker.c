#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <locale.h>
#include <regex.h>

#define D_INCLUDE_DIR L"/usr/include/"

struct data {
	wchar_t *desc;
	wchar_t *exec;
} data;

struct data **fext, **links, **domain, **filename, **regex;
int fexts, linkss, domains, filenames, regexs, dsize;
wchar_t *filemanager, *stdaction, *browser, *manpage, *arg;
wchar_t **idir;

struct data**
adddata(struct data **da, wchar_t *tail, int *size)
{
	if(da == NULL || tail == NULL)
		return NULL;

	struct data *d = malloc(sizeof(struct data));
	if(d == NULL)
		return NULL;

	wchar_t *tmp = malloc((wcslen(tail) + 1) * sizeof(wchar_t));
	if(tmp == NULL)
		return NULL;

	d->exec = wcscpy(tmp, tail);
	d->exec[wcslen(d->exec) - 1] = L'\0';
	da[*size - 1] = d;
	void *ret = realloc(da, (*size + 1)
		* sizeof(struct data*));
	if(ret == NULL)
		return NULL;

	*size+=1;
	return ret;
}

wchar_t*
setcomm(wchar_t *com)
{
	if(com == NULL)
		return NULL;

	wchar_t *tmp = malloc((wcslen(com) + 1)
		* sizeof(wchar_t));
	if(tmp == NULL)
		return NULL;

	str = wcscpy(tmp, com);
	str[wcslen(str) - 1] = L'\0';
	return str;
}

struct data*
selector(int x)
{
	struct data *d = NULL;
	if(x == 1)
		d = fext[fexts - 2];
	else if(x == 2)
		d = links[linkss - 2];
	else if(x == 3)
		d = domain[domains - 2];
	else if(x == 4)
		d = filename[filenames - 2];
	else if(x == 5)
		d = regex[regexs - 2];

	return d;
}

int
readconfig(char *file)
{
	if(file == NULL)
		return -1;

	FILE *fd = fopen(file, "r");
	if(fd == NULL)
		return -1;

	int size = 2;
	wchar_t c;
	wchar_t *line = malloc(size * sizeof(wchar_t));
	if(line == NULL)
		return -1;

	int i, j;
	int addflag = 0;
	while((c = fgetwc(fd)) != WEOF) {
		line[size - 2] = c;
		void *ret = realloc(line, (size + 1) * sizeof(wchar_t));
		if(ret == NULL)
			return -1;

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

		wchar_t *h = malloc((pos + 1) * sizeof(wchar_t));
		if(h == NULL)
				return -1;

		wchar_t *t = malloc((size - pos + 2) * sizeof(wchar_t));
		if(t == NULL)
			return -1;

		for(i = 0; i < pos; ++i)
			h[i] = line[i];

		j = 0;
		for(i = pos + 1; i < size - 1; ++i) {
			t[j] = line[i];
			j++;
		}

		h[pos] = L'\0';
		if(wcscmp(h, L"ext") == 0) {
			fext = adddata(fext, t, &fexts);
			addflag = 1;
		} else if(wcscmp(h, L"url") == 0) {
			links = adddata(links, t, &linkss);
			addflag = 2;
		} else if(wcscmp(h, L"domain") == 0) {
			domain = adddata(domain, t, &domains);
			addflag = 3;
		} else if(wcscmp(h, L"filename") == 0) {
			filename = adddata(filename, t, &filenames);
			addflag = 4;
		} else if(wcscmp(h, L"regex") == 0) {
			regex = adddata(regex, t, &regexs);
			addflag = 5;
		} else if(wcscmp(h, L"filemanager") == 0) {
			filemanager = setcomm(t);
			addflag = 0;
		} else if(wcscmp(h, L"stdaction") == 0) {
			stdaction = setcomm(t);
			addflag = 0;
		} else if(wcscmp(h, L"browser") == 0) {
			browser = setcomm(t);
			addflag = 0;
		} else if(wcscmp(h, L"manpage") == 0) {
			manpage = setcomm(t);
			addflag = 0;
		} else if(wcscmp(h, L"idir") == 0) {
			void *ret = realloc(idir, (dsize + 1)
				* sizeof(wchar_t*));
			if(ret == NULL)
				return -1;
			
			idir = ret;
			dsize++;
			wchar_t *tmp = malloc((wcslen(t) + 1)
				* sizeof(wchar_t));
			if(tmp == NULL)
				return -1;

			idir[dsize - 1] = wcscpy(tmp, t);
			addflag = 0;
		} else if(wcscmp(h, L"command") == 0) {
			struct data *d = selector(addflag);
			if(d != NULL) {
				wchar_t *tmp = malloc((wcslen(t) + 1)
					* sizeof(wchar_t));
				if(tmp == NULL)
					return -1;

				d->exec = wcscpy(tmp, t);
				d->exec[wcslen(d->exec) - 1] = L'\0';
			}
		} else {
			wprintf(L"unknown parameter: %ls\n", h);
			struct data *d = selector(addflag);
			if(d != NULL)
				d->exec = NULL;
		}

		free(h);
		free(t);

		cleanline:
		free(line);
		size = 2;
		line = malloc(size * sizeof(wchar_t));
		if(line == NULL)
			return -1;
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
	for(i = 0; esize > 0; ++i) {
		if((i == esize - 1)
		&& (arg[asize - i - 2] == L'.'))
			break;

		if(arg[asize - i - 1] != ext[esize - i - 1])
			return 0;
	}

	return 1;
}

wchar_t*
replace(wchar_t *str, wchar_t *placeholder, wchar_t *data
	,wchar_t *format,int extrasize)
{
	int i, j, k, size;
	int csize = wcslen(str);
	for(i = 0; i < csize; ++i) {
		if((str[i] == L'%')
		&& (i + 3 <= csize)
		&& (str[i + 1] == placeholder[0])
		&& (str[i + 2] == placeholder[1])
		&& (str[i + 3] == placeholder[2])) {
			wchar_t *head = malloc((i + 1)
				* sizeof(wchar_t));
			if(head == NULL)
				break;

			for(j = 0; j < i; ++j)
				head[j] = str[j];

			head[j] = L'\0';
			head = wcsncpy(head, str, i);
			wchar_t *tail = malloc(csize
				* sizeof(wchar_t));
			if(tail == NULL)
				break;

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
			if(cmd == NULL)
				break;

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
evaluate(wchar_t *arg)
{
	int i, j, k;
	int asize = wcslen(arg);
	int csize;
	int freearg = 0;
	wchar_t *linenum = NULL;
	wchar_t *command = NULL;
	wchar_t *cmd = NULL;
	wchar_t *tmp = NULL;
	for(i = asize; i > 0; --i) {
		if(arg[i] == L':') {
			j = asize - i;
			linenum = malloc((j + 1) * sizeof(wchar_t));
			if(linenum == NULL)
				break;

			for(k = 0; k < j; ++k)
				linenum[k] = arg[i + k + 1];

			linenum[wcslen(linenum)] = L'\0';
			csize = asize - wcslen(linenum);
			wchar_t *carg = malloc((csize + 1)
				* sizeof(wchar_t));
			if(carg == NULL)
				break;

			for(k = 0; arg[k] != L':'; ++k)
			carg[k] = arg[k];

			carg[k] = L'\0';
			arg = carg;
			asize = wcslen(carg);
			freearg = 1;
			break;
		}

		if(arg[i] == L'.'
		|| arg[i] == L'/')
			break;
	}

	if(linenum == NULL) {
		linenum = malloc(2 * sizeof(wchar_t));
		if(linenum == NULL)
			return;

		linenum[0] = L'1';
		linenum[1] = L'\0';
	}

	for(i = 0; i < regexs; ++i) {
		struct data *d = regex[i];
		if(d == NULL)
			break;

		char *pattern = malloc((wcslen(d->desc) + 1)
			* sizeof(char));
		if(pattern == NULL)
			break;

		wcstombs(pattern, d->desc, wcslen(d->desc));
		pattern[wcslen(d->desc)] = '\0';
		regex_t reg;
		int ret = regcomp(&reg, pattern, REG_EXTENDED);
		free(pattern);
		if(ret > 0)
			continue;

		char *carg = malloc((wcslen(arg) + 1) * sizeof(char));
		if(carg == NULL)
			break;

		wcstombs(carg, arg, wcslen(arg));
		carg[strlen(carg)] = '\0';
		ret = regexec(&reg, carg, 0, NULL, 0);
		free(carg);
		if(ret == 0) {
			regfree(&reg);
			command = d->exec;
			goto evaluation;
		}

		regfree(&reg);
	}

	if((arg[0] == L'<')
	&& (arg[asize - 1] == L'>')) {
		for(i = 0; i < dsize; ++i) {
			wchar_t *ipath = idir[i];
			if(ipath[wcslen(ipath) - 1] == L'\n')
				ipath[wcslen(ipath) - 1] = L'\0';

			int psize = wcslen(ipath) + wcslen(arg);
			wchar_t *p = malloc(psize * sizeof(wchar_t));
			if(p == NULL)
				return;

			wchar_t *fn = malloc(asize * sizeof(wchar_t));
			if(fn == NULL)
				return;

			for(j = 1; j < asize - 1; ++j)
				fn[j - 1] = arg[j];

			fn[j - 1] = '\0';
			swprintf(p, psize, L"%ls%ls", ipath, fn);
			free(fn);
			char *cp = malloc((psize + 1) * sizeof(char));
			if(cp == NULL)
				return;

			wcstombs(cp, p, psize);
			int ret = access(cp, F_OK);
			free(cp);
			if(ret == 0) {
				if(freearg == 1)
					free(arg);
				else
					freearg = 1;

				arg = p;
				break;
			}

			free(p);
		}
	}

	if((arg[0] == L'\"')
	&& (arg[asize - 1] == L'\"')) {
		wchar_t *p = malloc((asize + 1) * sizeof(wchar_t));
		if(p == NULL)
			return;

		p[0] = L'.';
		p[1] = L'/';
		for(i = 2; i < asize; ++i)
			p[i] = arg[i - 1];

		if(freearg == 1)
			free(arg);
		else
			freearg = 1;

		arg = p;
	}

	if(!(wcsncmp(arg, L"http", 4))
	|| !(wcsncmp(arg, L"https", 5))) {
		for(i = 0; i < linkss; ++i) {
			struct data *d = links[i];
			if(d == NULL)
				break;

			if(wcsncmp(arg, d->desc, wcslen(d->desc)) == 0) {
				command = d->exec;
				goto evaluation;
			}
		}

		for(i = 0; i < domains; ++i) {
			struct data *d = domain[i];
			if(d == NULL)
				break;

			int begin, end;
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
	|| (arg[0] == L'.' && arg[1] == L'/')) {
		for(i = 0; i < filenames; ++i) {
			struct data *d = filename[i];
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
		for(i = wcslen(arg) - 2; i > 0; i--) {
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

		for(i = 0; i < fexts; ++i) {
			struct data *d = fext[i];
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
		if(sec == NULL)
			return;

		sec[0] = arg[asize - 2];
		sec[1] = L'\0';
		command = manpage;
		wchar_t *tmp = replace(command, L"sec", sec, L"%ls%ls%ls"
			,1);
		if(tmp != NULL) {
			free(cmd);
			cmd = tmp;
			command = cmd;
		}

		wchar_t *newarg = malloc(asize * sizeof(wchar_t));
		if(newarg == NULL)
			return;

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

		free(sec);
		arg = newarg;
		asize = wcslen(arg);
	}
	
	if(command == NULL)
		return;

	evaluation:
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
	if(scommand == NULL)
		return;

	wcstombs(scommand, command, (wcslen(command) + 2));
	system(scommand);
	free(scommand);
	free(linenum);
	free(cmd);
	if(freearg == 1)
		free(arg);
}

void
freedata(struct data **a, int size)
{
	if(a == NULL)
		return;

	int i;
	for(i = 0; i < size - 1; ++i) {
		struct data *d = a[i];
		free(d->desc);
		free(d->exec);
		free(d);
	}
}

int
main(int argc, char **argv)
{
	setlocale(LC_ALL, getenv("LANG"));
	pid_t pid = 0;
	wchar_t c;
	int size = 2;
	arg = malloc(size * sizeof(wchar_t));
	if(arg == NULL)
		return -1;

	while((c = fgetwc(stdin)) != L'\n') {
		if((size == 2 && c == L' ')
		|| (size == 2 && c == L'\t'))
			continue;

		arg[size - 2] = c;
		void *ret = realloc(arg, (size + 1) * sizeof(wchar_t));
		if(ret == NULL)
			return -1;

		size++;
		arg = ret;
		arg[size - 2] = L'\0';
	}

	if(size > 2) {
		int i;
		char *home = getenv("HOME");
		if(home == NULL)
			return -1;

		char *cname = ".joker";
		int size = strlen(home)
			+ strlen(cname)
			+ 2;
		char *path = malloc(size * sizeof(char));
		if(path == NULL)
			return -1;

		snprintf(path, size, "%s/%s", home, cname);
		fexts = 1;
		linkss = 1;
		domains = 1;
		filenames = 1;
		regexs = 1;
		fext = calloc(fexts, sizeof(struct data*));
		links = calloc(linkss, sizeof(struct data*));
		domain = calloc(domains, sizeof(struct data*));
		regex = calloc(regexs, sizeof(struct data*));
		filename = calloc(filenames, sizeof(struct data*));
		if(fext == NULL || links == NULL || domain == NULL
		|| filename == NULL || regex == NULL)
			return -1;

		filemanager = NULL;
		stdaction = NULL;
		browser = NULL;
		manpage = NULL;
		dsize = 1;
		idir = calloc(dsize, sizeof(wchar_t*));
		if(idir == NULL)
			return -1;

		wchar_t *tmp = malloc((wcslen(D_INCLUDE_DIR) + 1)
			* sizeof(wchar_t));
		if(tmp == NULL)
			return -1;

		idir[0] = wcscpy(tmp, D_INCLUDE_DIR);
		pid = fork();
		if(pid == 0) {
			if(readconfig(path) == 0)
				evaluate(arg);
		}

		for(i = 0; i < dsize; ++i)
			free(idir[i]);

		free(idir);
		free(path);
		freedata(fext, fexts);
		freedata(links, linkss);
		freedata(domain, domains);
		freedata(filename, filenames);
		freedata(regex, regexs);
		free(fext);
		free(domain);
		free(links);
		free(regex);
		free(filename);
		free(filemanager);
		free(stdaction);
		free(browser);
		free(manpage);
	}

	free(arg);
	return 0;
}
