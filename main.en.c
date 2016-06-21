/*
	Developed by Eduardo A. Pacheco
	"For of him, and through him, and to him, are all things:
	to whom be glory for ever. Amen." (Romans 11:36 KJV)

	June 21, 2016 - Brazil

	------------------------------------------

	main.c
	This brief algorithm has the purpose of testing every functionality
	of the B*-Tree, those being: insert, search, remove
	It also closes the tree a few times in order to properly certify
	the correctness of the written files.
*/

#include <time.h>
#include <ctype.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* sysconf(3) */

#include "bs-tree.h"

#define BLOCKSIZE 64
#define INDEXPATH "btree.data"

int paircmp (char *searchkey, char *reckey);
void printkey (char *key, size_t recoffset, va_list vl);
void printkey_only (char *key);
void strshuffle (char*);
int test_btree ();
int test_btree_print ();

int main(int argc, char const *argv[]) {
	test_btree();
//	test_btree_print();
}

int test_btree ()
{
	int i;
	BSTREE *t;

	if ( !(t = bstcreate (INDEXPATH, BLOCKSIZE, sizeof(char), (compare_type) paircmp)) )
		return 1;

//	int degree = btcalcdegree (BLOCKSIZE, sizeof(PAIR));
	printf("degree (%lu, %lu): %d\n\n", bstgetblocksize(t), bstgetkeysize(t), bstgetdegree(t));

	char debug_set[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890";
	strshuffle(debug_set);
	char c;

	printf("insertion\n");
	for (i = 0; (c = debug_set[i]) != '\0'; ++i)
//	while ((c = getchar()) != EOF && c != '\n' && ++i)
	{
//		if ( isspace(c) )
//			continue;
		printf("%c(%d) ", c, i);
		bstinsert (t, &c, (size_t) i);
	}
	printf("\n\n");
	printf("inorder: ");
	__bstree_debug_dfs (t, NULL, (runkey_type) printkey, NULL);
	printf("\n\n");
	bstclose(t);

	printf("reloading tree ...\n\n");
	t = bstopen(INDEXPATH, (compare_type) paircmp);

	printf("partial delete\n");
	for (i = 0; i < strlen(debug_set); i += 2)
	{
		size_t recoffset = bstdelete(t, debug_set + i);
		printf ("%c(%lu) ", debug_set[recoffset], recoffset);
	}
	printf("\n");
	for (i = 0; i < strlen(debug_set); i += 2)
	{
		size_t recoffset = bstsearch(t, debug_set + i);
		if (recoffset != ULONG_MAX)
			printf("********* search failed\n");
	}
	printf("found:\n");
	for (i = 1; i < strlen(debug_set); i += 2)
	{
		size_t recoffset = bstsearch(t, debug_set + i);
		if (recoffset == ULONG_MAX)
			printf("********* search failed\n");
		else
			printf("%c(%lu) ", debug_set[recoffset], recoffset);
	}
	printf("\n");
	printf("re-inserting\n");
	for (i = 0; i < strlen(debug_set); i += 2)
	{
		bool ok = bstinsert(t, debug_set + i, i);
		if (!ok)
			printf("******** insert failed\n");
	}
	printf("\n");
	bstclose(t);


	printf("reloading tree ...\n");
	t = bstopen(INDEXPATH, (compare_type) paircmp);
	printf("inorder: ");
	__bstree_debug_dfs (t, NULL, (runkey_type) printkey, NULL);
	printf("\n\n");
	strshuffle(debug_set);
	for (i = 0; (c = debug_set[i]) != '\0'; ++i)
//	while ((c = getchar()) != EOF && c != '\n')
	{
		if ( isspace(c) )
			continue;

		printf("[%c] ", c);

		size_t recoffset = bstdelete(t, &c);

		if (recoffset == ULONG_MAX)
			printf("********* [character '%c' not found]\n", c);

		printf("inorder: ");
		__bstree_debug_dfs (t, NULL, (runkey_type) printkey, NULL);
		printf("\n\n");
	}
	bstclose(t);
	return 0;
}

int test_btree_print ()
{
	int i;
	BSTREE *t;

	if ( !(t = bstcreate (INDEXPATH, BLOCKSIZE, sizeof(char), (compare_type) paircmp)) )
		return 1;

	printf("degree (%lu, %lu): %d\n\n", bstgetblocksize(t), bstgetkeysize(t), bstgetdegree(t));

//	char debug_set[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890";
//	strshuffle(debug_set);
//	for (i = 0; debug_set[i] != '\0'; ++i)
//		bstinsert (t, debug_set + i, (size_t) i);

	for (i = 0; i < 94/*111*/; ++i)
	{
		char c = i + 33;
		printf("%d%c|", i, c);
		bstinsert (t, &c, (size_t) i);
	}
	putchar('\n');

//	freopen ("myfile.txt", "w" ,stdout);
	__bstree_debug_printtree(t, (printkey_type) printkey_only);
	putchar('\n');

	freopen ("myfile.txt","w",stdout);
	__bstree_debug_printtree(t, (printkey_type) printkey_only);
	putchar('\n');

	bstclose(t);
	return 0;
}


void strshuffle (char *arr) {
	int i;
	unsigned long len = strlen(arr);

	srand( (unsigned int) time(NULL) );
	for (i = 0; i < len; ++i)
	{
		char aux = arr[i];
		unsigned long pos = rand() % len;
		arr[i] = arr[pos];
		arr[pos] = aux;
	}
}

void printkey_only (char *key) {
	printf("%d", (int) *key);
}

void printkey (char *key, size_t recoffset, va_list vl) {
	printf("%c ", *key);
}

int paircmp (char *searchkey, char *reckey) {
	return *searchkey - *reckey;
}