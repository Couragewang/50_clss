#include <iostream>
#include <string>
#include <vector>
#include <time.h>
#include <pthread.h>
using namespace std;

int main()
{
    pthread_mutex_t lock;
    pthread_cond_t cond;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);


    int ret = pthread_cond_timedwait(&cond, &lock, &ts);
    cout << ret << endl;
}
