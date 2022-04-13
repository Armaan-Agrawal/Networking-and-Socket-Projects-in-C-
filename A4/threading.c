#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

//void foo(int arg)
void* foo(void *arg)
{
	int par = *(int *)(arg);

	for(int i=0; i<10; i++)
	{
		printf("Call %d: %d \n",par,i);
		usleep(10000);
	}
	return NULL;
}

int main()
{
	
	int par1 =1;
	int par2 =2;
	pthread_t t_id1, t_id2;

	pthread_create(&t_id1,NULL,foo,&par1);
	pthread_create(&t_id2,NULL,foo,&par2);


	pthread_join(t_id1,NULL);
	pthread_join(t_id2,NULL);
	//foo(1);
	//foo(2);

	return 0;
}