#include <stdlib.h>
#include "item.h"
#include "st.h"

#include <stdio.h>

typedef struct STnode *link;
struct STnode {
	Item item;
	link l;
	link r;
	int N;
};
static link head, z;

link NEW(Item item, link l, link r, int N) {
	link x = malloc(sizeof(*x));
	x->item = item;
	x->l = l;
	x->r = r;
	x->N = N;
	return x;
}

link STrotR(link h) {
	link x = h->l;
	h->l = x->r;
	x->r = h;

	h->N = h->r->N + h->l->N + 1;
	x->N = x->r->N + x->l->N + 1;
	
	return x;
}

link STrotL(link h) {
	link x = h->r;
	h->r = x->l;
	x->l = h;

	h->N = h->r->N + h->l->N + 1;
	x->N = x->r->N + x->l->N + 1;
	
	return x;
}

link STpartR(link h, int k) {
	int t = h->l->N;
	
	if (t > k) {
		h->l = STpartR(h->l, k);
		h = STrotR(h);
	}
	
	if (t < k) {
		h->r = STpartR(h->r, k-t-1);
		h = STrotL(h);
	}
	
	return h;
}

link STjoinLR(link a, link b) {
	if (b == z) {
		return a;
	}
	
	b = STpartR(b, 0);
	b->l = a;
	
	b->N = b->r->N + b->l->N + 1;
	
	return b;
}

link STbalanceR(link h) {
	if (h->N < 2) {
		return h;
	}
	
	h = STpartR(h, h->N/2);
	
	h->l = STbalanceR(h->l);
	h->r = STbalanceR(h->r);
	
	h->N = h->r->N + h->l->N + 1;
	
	return h;
}

void STinit(void) {
	head = (z = NEW(NULLitem, 0, 0, 0));
}

int STcount(void) {
	return head->N;
}

Item STsearchR(link h, Key v) {
	Key t;
	
	if (h == z) {
		return NULLitem;
	}
	
	t = key(h->item);
	
	if (eqkey(v, t)) {
		return h->item;
	}
	
	if (lesskey(v, t)) {
		return STsearchR(h->l, v);
	} else {
		return STsearchR(h->r, v);
	}
}

Item STsearch(Key v) {
	return STsearchR(head, v);
}

link STinsertT(link h, Item item) {
	Key v;
	
	if (h == z) {
		return NEW(item, z, z, 1);
	}
	
	v = key(item);

	if (lesskey(v, key(h->item))) {
		h->l = STinsertT(h->l, item);
		(h->N)++;
		h = STrotR(h);
	} else {
		h->r = STinsertT(h->r, item);
		(h->N)++;
		h = STrotL(h);
	}
	
	return h;
}

link STsplayinsert(link h, Item item) {
	Key v = key(item);
	
	if (h == z) {
		return NEW(item, z, z, 1);
	}
	
	if (lesskey(v, key(h->item))) {
		if (h->l == z) {
			return NEW(item, z, h, h->N+1);
		}
		if (lesskey(v, key(h->l->item))) {
			h->l->l = STsplayinsert(h->l->l, item);
			h = STrotR(h);
		} else {
			h->l->r = STsplayinsert(h->l->r, item);
			h->l = STrotL(h->l);
		}
		return STrotR(h);
	} else {
		if (h->r == z) {
			return NEW(item, h, z, h->N+1);
		}
		if (lesskey(key(h->r->item), v)) {
			h->r->r = STsplayinsert(h->r->r, item);
			h = STrotL(h);
		} else {
			h->r->l = STsplayinsert(h->r->l, item);
			h->r = STrotR(h->r);
		}
		return STrotL(h);
	}
}

void STinsert(Item item) {
	head = STsplayinsert(head, item);
}

Item STselectR(link h, int k) {
	int t;
	
	if (h == z) {
		return NULLitem;
	}
	
	t = (h->l == z) ? 0 : h->l->N;
	
	if (t > k) {
		return STselectR(h->l, k);
	}
	
	if (t < k) {
		return STselectR(h->r, k-t-1);
	}
	
	return h->item;
}

Item STselect(int k) {
	return STselectR(head, k);
}

link STdeleteR(link h, Key v) {
	int p = 0;
	link x;
	Key t = key(h->item);
	
	if (h == z) {
		return z;
	}
	
	if (lesskey(v, t)) {
		if (h->l != z) {
			p = 1;
		}
		h->l = STdeleteR(h->l, v);
		if (h->l != z || (p == 1 && h->l == z)) {
			--(h->N);
		}
	}
	
	if (lesskey(t, v)) {
		if (h->r != z) {
			p = 1;
		}
		h->r = STdeleteR(h->r, v);
		if (h->r != z || (p == 1 && h->r == z)) {
			--(h->N);
		}
	}
	
	if (eqkey(v, t)) {
		x = h;
		h = STjoinLR(h->l, h->r);
		free(x);
	}
	
	return h;
}

void STdelete(Key v) {
	head = STdeleteR(head, v);
}

void STsortR(link h, void (*visit)(Item)) {
	if (h == z) {
		return;
	}
	
	STsortR(h->l, visit);
	visit(h->item);
	STsortR(h->r, visit);
}

void STsort(void (*visit)(Item)) {
	STsortR(head, visit);
}


/*
 * Print tree functions
 */


static void printnode(Key x, int n, int h) {
	int i;

	for (i = 0; i < h; ++i) {
		printf("    ");
	}

	//printf("%c(%i)\n", x, n);
	printf("%c\n", x);
}

static void showTree(link x, int h) {
	if (x == z) {
		printnode('*', 0, h);
		return;
	}

	showTree(x->r, h+1);

	printnode(x->item->key, x->N, h);

	showTree(x->l, h+1);
}

void STdrawtree(void) {
	showTree(head, 0);
}
