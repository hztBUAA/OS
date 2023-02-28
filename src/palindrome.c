#include <stdio.h>
char a[1000];
int isTrue(int n);
int main() {
	int n;
	scanf("%d", &n);

	if (isTrue(n)==1) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
int isTrue(int n){
	int i = 0;
	while(n!=0){
		a[i++]='0'+n%10;
		n=n/10;
	}
	int flag = 0;
	int l = i-1;
	for(i=i-1;i>=0;i--){
		if(a[l-i]!=a[i]){
			flag = 1;
			return 0;
		}
	}
	if(!flag){
		return 1;
	}
}

