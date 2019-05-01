#include <string.h>
#include <stdio.h>

int main(void){
	char* s = "hello guy";
	char *st[] = {
		"sup man",
		"bye"
	};
	char* token;
	int i = 0;
	char *cp[5];
	/*
	while(i < 4){
		cp[i] = strsep(&s, " ");
		if(strlen(cp[i]) == 0) break;
		printf("%s\n", cp[i]);
		++i;
	}*/
	for(int i = 0; i < 4; ++i){
		cp[i] = strsep(&s, " ");
		if (cp[i] == NULL) break;
	}
	//printf("%ld\n", strlen(cp[1]));
	//printf("%ld\n", strlen(cp[0]));
	/*
	if(cp[0] == NULL) printf("wuti\n");
	cp[0] = st[0];
	printf("%s\n", st[0]);
	printf("%s\n", cp[0]);*/
}
