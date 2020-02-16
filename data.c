#include "data.h"

struct data*
datanew()
{
	struct data *d = malloc(sizeof(struct data));
	if(d == NULL)
		return NULL;

	d->desc = NULL;
	d->exec = NULL;
	return d;
}

int
datadestroy(struct data *d)
{
	if(d == NULL)
		return -1;

	free(d->desc);
	free(d->exec);
	free(d);
	return 0;
}

int
datainsert(struct data *d, wchar_t *data, int field)
{
	if(data == NULL || field == 0)
		return -1;

	if(field == 1)
		d->desc = data;
	else if(field == 2)
		d->exec = data;
	else
		return -2;

	return 0;
}

struct container*
containernew()
{
	struct container *cont = malloc(sizeof(struct container));
	if(cont == NULL)
		return NULL;

	cont->d = calloc(1, sizeof(struct data*));
	if(cont->d == NULL) {
		free(cont);
		return NULL;
	}

	cont->size = 1;
	return cont;
}

int
containerdestroy(struct container *cont)
{
	if(cont == NULL)
		return - 1;

	int i;
	for(i = 0; i < cont->size - 1; ++i) {
		if((datadestroy(cont->d[i])) != 0)
			wprintf(L"data error\n");
	}

	free(cont->d);
	free(cont);
	return 0;
}

int
containerinsert(struct container *cont, struct data *d)
{
	if(cont == NULL || d == NULL)
		return -1;

	cont->d[cont->size - 1] = d;
	struct data **ret = realloc(cont->d, (cont->size + 1)
		* sizeof(struct data*));
	if(ret == NULL)
		return -2;

	cont->d = ret;
	cont->size+=1;
	cont->d[cont->size - 1] = NULL;
	return 0;
}

struct wlist*
wlistnew()
{
	struct wlist *wl = malloc(sizeof(struct wlist));
	if(wl == NULL)
		return NULL;

	wl->list = calloc(1, sizeof(wchar_t*));
	if(wl->list == NULL) {
		free(wl);
		return NULL;
	}

	wl->size = 1;
	return wl;
}

int
wlistdestroy(struct wlist *wl)
{
	if(wl == NULL)
		return -1;
	int i;
	for(i = 0; i < wl->size - 1; ++i)
		free(wl->list[i]);

	free(wl->list);
	free(wl);
	return 0;
}

int
wlistinsert(struct wlist *wl, wchar_t *data)
{
	if(wl == NULL || data == NULL)
		return -1;

	wl->list[wl->size - 1] = data;
	wchar_t **ret = realloc(wl->list, (wl->size + 1)
		* sizeof(wchar_t*));
	if(ret == NULL)
		return -1;
			
	wl->list = ret;
	wl->size+=1;
	wl->list[wl->size - 1] = NULL;
	return 0;
}

