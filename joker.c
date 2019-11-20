#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <locale.h>
#include <regex.h>

struct data {
	wchar_t *desc;
	wchar_t *exec;
} data;

struct data **fext, **links, **domain, **filename, **regex;
int fexts, linkss, domains, filenames, regexs;
wchar_t *filemanager, *stdaction, *browser, *arg;

struct data**
adddata(struct data **da, wchar_t *tail, int *size)
{
	if(da == NULL || tail == NULL)
		return NULL;

	struct data *d = malloc(sizeof(struct data));
	if(d == NULL)
		return NULL;

	d->desc = malloc((wcslen(tail) + 1) * sizeof(wchar_t));
	if(d->desc == NULL)
		return NULL;

	d->exec = wcscpy(d->desc, tail);
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

	wchar_t *str = malloc((wcslen(com) + 1)
		* sizeof(wchar_t));
	if(str == NULL)
		return NULL;

	str = wcscpy(str, com);
	str[wcslen(str) - 1] = L'\0';
	return str;
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
		}

		if(wcscmp(h, L"url") == 0) {
			links = adddata(links, t, &linkss);
			addflag = 2;
		}

		if(wcscmp(h, L"domain") == 0) {
			domain = adddata(domain, t, &domains);
			addflag = 3;
		}

		if(wcscmp(h, L"filename") == 0) {
			filename = adddata(filename, t, &filenames);
			addflag = 4;
		}

		if(wcscmp(h, L"regex") == 0) {
			regex = adddata(regex, t, &regexs);
			addflag = 5;
		}

		if(wcscmp(h, L"filemanager") == 0) {
			filemanager = setcomm(t);
			addflag = 0;
		}

		if(wcscmp(h, L"stdaction") == 0) {
			stdaction = setcomm(t);
			addflag = 0;
		}

		if(wcscmp(h, L"browser") == 0) {
			browser = setcomm(t);
			addflag = 0;
		}

		if(wcscmp(h, L"command") == 0) {
			struct data *d = NULL;
			if(addflag == 1)
				d = fext[fexts - 2];

			if(addflag == 2)
				d = links[linkss - 2];

			if(addflag == 3)
				d = domain[domains - 2];

			if(addflag == 4)
				d = filename[filenames - 2];

			if(addflag == 5)
				d = regex[regexs - 2];

			if(d == NULL)
				break;

			d->exec = malloc((wcslen(t) + 1) * sizeof(wchar_t));
			if(d->exec == NULL)
				return -1;

			d->exec = wcscpy(d->exec, t);
			d->exec[wcslen(d->exec) - 1] = L'\0';
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
		if(arg[asize - (esize + 1)] != ext[i])
			return 0;

		esize--;
	}

	return 1;
}

void
evaluate(wchar_t *arg)
{
	int i, j, k;
	int size = 0;
	int asize = wcslen(arg);
	int csize;
	int freearg = 0;
	wchar_t *linenum = NULL;
	wchar_t *command = NULL;
	wchar_t *cmd = NULL;
	for(i = asize; i > 0; --i) {
		if(arg[i] == L':') {
			j = asize - i;
			linenum = malloc((j + 1) * sizeof(wchar_t));
			if(linenum == NULL)
				break;

			for(k = 0; k < j; ++k)
				linenum[k] = arg[i + k + 1];

			linenum[wcslen(linenum) - 1] = L'\0';
			csize = asize - wcslen(linenum);
			wchar_t *carg = malloc((csize + 1) * sizeof(wchar_t));
			if(carg == NULL)
				break;

			for(k = 0; k < csize; ++k)
				carg[k] = arg[k];

			carg[csize - 1] = L'\0';
			arg = carg;
			asize = wcslen(carg);
			arg[csize - 1] = L'\0';
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

		char *pattern = malloc((wcslen(d->desc) + 1) * sizeof(char));
		if(pattern == NULL)
			break;

		wcstombs(pattern, d->desc, wcslen(d->desc) + 1);
		pattern[strlen(pattern) - 1] = '\0';
		regex_t reg;
		int ret = regcomp(&reg, pattern, REG_EXTENDED);
		free(pattern);
		if(ret > 0)
			continue;

		char *carg = malloc((wcslen(arg) + 1) * sizeof(char));
		if(carg == NULL)
			break;

		wcstombs(carg, arg, wcslen(arg) + 1);
		carg[strlen(carg) - 1] = '\0';
		ret = regexec(&reg, carg, 0, NULL, 0);
		free(carg);
		if(ret == 0) {
			regfree(&reg);
			command = d->exec;
			goto evaluation;
		}

		regfree(&reg);
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

		if(arg[asize - 2] == L'/') {
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
	}

	if(command == NULL)
		return;

	evaluation:
	arg[wcslen(arg) - 1] = L'\0';
	csize = wcslen(command);
	for(i = 0; i < csize; ++i) {
		if((command[i] == L'%')
		&& (i + 3 <= csize)
		&& (command[i + 1] == L'a')
		&& (command[i + 2] == L'r')
		&& (command[i + 3] == L'g')) {
			wchar_t *head = malloc((i + 2)
				* sizeof(wchar_t));
			if(head == NULL)
				break;

			for(j = 0; j < i; ++j)
				head[j] = command[j];

			head[i] = L'\0';
			wchar_t *tail = malloc(csize
				* sizeof(wchar_t));
			if(tail == NULL)
				break;

			k = 0;
			for(j = i + 4; j < csize; ++j) {
				tail[k] = command[j];
				k++;
			}

			tail[k] = L'\0';
			size = wcslen(head)
				+ wcslen(arg)
				+ wcslen(tail)
				+ 3;
			wchar_t *cmd = malloc(size * sizeof(wchar_t));
			if(cmd == NULL)
				break;

			swprintf(cmd, size, L"%ls'%ls'%ls"
				,head
				,arg
				,tail);
			command = cmd;
			free(head);
			free(tail);
		}
	}

	csize = wcslen(command);
	for(i = 0; i < csize; ++i) {
		if((command[i] == L'%')
		&& (i + 4 <= csize)
		&& (command[i + 1] == L'l')
		&& (command[i + 2] == L'i')
		&& (command[i + 3] == L'n')
		&& (command[i + 4] == L'e')) {
			wchar_t *head = malloc((i + 2) * sizeof(wchar_t));
			if(head == NULL)
				break;

			for(j = 0; j < i; ++j)
				head[j] = command[j];

			head[i] = L'\0';
			wchar_t *tail = malloc(csize
				* sizeof(wchar_t));
			if(tail == NULL)
				break;

			k = 0;
			for(j = i + 5; j < csize; ++j) {
				tail[k] = command[j];
				k++;
			}

			tail[k] = L'\0';
			size = wcslen(head)
				+ wcslen(linenum)
				+ wcslen(tail)
				+ 1;
			wchar_t *commandc = malloc(size
				* sizeof(wchar_t));
			if(commandc == NULL)
				break;

			swprintf(commandc, size, L"%ls%ls%ls"
				,head
				,linenum
				,tail);
			free(cmd);
			cmd = commandc;
			command = cmd;
			free(head);
			free(tail);
		}
	}

	char *scommand = malloc((wcslen(command) + 1)
		* sizeof(char));
	if(scommand == NULL)
		return;

	wcstombs(scommand, command, (wcslen(command) + 1));
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

	while((c = fgetwc(stdin)) != WEOF) {
		if((size == 2 && c == L' ')
		|| (size == 2 && c == L'\t'))
			continue;

		arg[size - 2] = c;
		void *ret = realloc(arg, (size + 1) * sizeof(wchar_t));
		if(ret == NULL)
			return -1;

		size++;
		arg = ret;
		arg[size - 1] = L'\0';
	}

	if(size > 2) {
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
		pid = fork();
		if(pid == 0) {
			if(readconfig(path) == 0)
				evaluate(arg);
		}

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
	}

	free(arg);
	return 0;
}
