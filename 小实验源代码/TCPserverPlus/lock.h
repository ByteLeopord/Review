#ifndef LOCK_H
#define LOCK_H
#include<iostream>
#include<pthread.h>
#include<semaphore.h>

using namespace std;

class Sem{
private:
	sem_t sem;
public:
	Sem();
	~Sem();
	bool wait();
	bool post();
};

Sem::Sem(){
	if(sem_init(&sem,0,0) != 0)
		cerr << "sem init error" << endl;
}

Sem::~Sem(){
	sem_destroy(&sem);
}

bool Sem::wait(){
	return sem_wait(&sem) == 0 ? true : false;
}

bool Sem::post(){
	return sem_post(&sem) == 0 ? true : false;
}

class Mutex{
private:
	pthread_mutex_t mutex;
public:
	Mutex();
	~Mutex();
	bool mutex_lock();
	bool mutex_unlock();
};

Mutex::Mutex(){
	if(pthread_mutex_init(&mutex,NULL) != 0)
		cerr << "mutex init error" << endl;
}

Mutex::~Mutex(){
	pthread_mutex_destroy(&mutex);
}

bool Mutex::mutex_lock(){
	return pthread_mutex_lock(&mutex) == 0 ? true : false;
}

bool Mutex::mutex_unlock(){
	return pthread_mutex_unlock(&mutex) == 0 ? true : false;
}

class Cond{
private:
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	
public:
	Cond();
	~Cond();
	bool wait();
	bool signal();
	bool broadcast();
};

Cond::Cond(){
	if(pthread_mutex_init(&mutex,NULL) != 0){
		cerr << "Cond mutex init error" << endl;
		exit(0);
	}
	if(pthread_cond_init(&cond,NULL) != 0){
		cerr << "Cond cond init error" << endl;
		exit(0);
	}
}

Cond::~Cond(){
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
}

bool Cond::wait(){
	int rs = 0;
	pthread_mutex_lock(&mutex);
	rs = pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex);
	return rs == 0 ? true : false;
}

bool Cond::signal(){
	return pthread_cond_signal(&cond) == 0 ? true : false;
}

bool Cond::broadcast(){
	return pthread_cond_broadcast(&cond);
}


#endif
