#ifndef DATA_H
#define DATA_H

#include <wchar.h>
#include <stdlib.h>

struct data {
	wchar_t *desc;
	wchar_t *exec;
} data;

struct container {
	int size;
	struct data **d;
} container;

struct wlist {
	int size;
	wchar_t **list;
} wlist;

struct data*
datanew();

int
datadestroy(struct data*);

int
datainsert(struct data*, wchar_t*, int);


struct container*
containernew();

int
containerdestroy(struct container *cont);

int
containerinsert(struct container *cont, struct data *d);


struct wlist*
wlistnew();

int
wlistdestroy(struct wlist *wl);

int
wlistinsert(struct wlist *wl, wchar_t *data);

#endif

