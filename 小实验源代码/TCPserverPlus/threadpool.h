#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<queue>
#include<vector>
#include<exception>
#include<errno.h>
#include"lock.h"
#define MAX_THREADS 1024

using namespace std;

template<class T>
class Threadpool{
private:
	int idle;
	int num;
	vector<pthread_t> idle_tid;
	vector<pthread_t> busy_tid;
	queue<T *> task_queue;
	Mutex mutex;
	Cond cond;
	bool is_stop;

public:
	static void* worker(void* arg);
	void run();
	T* get_task();
	int mv_to_idle(pthread_t tid);
	int mv_to_busy(pthread_t tid);
	
public:
	Threadpool(int n = 20);
	~Threadpool();
	bool append_task(T* task);
	void start();
	void stop();
};

template<class T>
Threadpool<T>::Threadpool(int n):num(n), idle(n), is_stop(false){
	if(num <= 0){
		cerr << "threadpool cant init" << endl;
		exit(1);
	}
}

template<class T>
Threadpool<T>::~Threadpool(){
	stop();
}

template<class T>
bool Threadpool<T>::append_task(T* task){
	mutex.mutex_lock();
	
	bool is_signal = task_queue.empty();
	task_queue.push(task);
	
	mutex.mutex_unlock();
	
	if(is_signal){
		cond.signal();
	}
	
	return true;
}

template<class T>
void Threadpool<T>::start(){
	for(int i = 0; i < num; ++i){
		pthread_t tid = 0;
		if(pthread_create(&tid,NULL,worker,this) != 0){
			throw exception();
			exit(1);
		}
		idle_tid.push_back(tid);
	}
}

template<class T>
void Threadpool<T>::stop(){
	is_stop = true;
	cond.broadcast();
}

template<class T>
void* Threadpool<T>::worker(void* arg){
	Threadpool<T> *thread = (Threadpool<T>*)arg;
	thread->run();
	return thread;
}

template<class T>
void Threadpool<T>::run(){
	pthread_t tid = pthread_self();
	while(1){
		T* task = get_task();
		if(task == NULL){
			cerr << "task_queue is null.wait()" << endl;
			cond.wait();
		}
		else{
			mv_to_busy(tid);
			task->doit();
			mv_to_idle(tid);
		}
	}
}

template<class T>
T* Threadpool<T>::get_task(){
	T* task = NULL;
	
	mutex.mutex_lock();
	if(!task_queue.empty()){
		task = task_queue.front();
		task_queue.pop();
	}
	mutex.mutex_unlock();
	
	return task;
}

template<class T>
int Threadpool<T>::mv_to_idle(pthread_t tid){
	vector<pthread_t>::iterator busy_iter = busy_tid.begin();
	while(busy_iter != busy_tid.end()){
		if(tid == *busy_iter)
	   		break;
		++busy_iter;
	}    

	mutex.mutex_lock();
	busy_tid.erase(busy_iter);
	idle_tid.push_back(tid);

	++idle;
	mutex.mutex_unlock();
	return 0;
}

template<class T>
int Threadpool<T>::mv_to_busy(pthread_t tid){
	vector<pthread_t>::iterator idle_iter = idle_tid.begin();
	while(idle_iter != idle_tid.end()){
		if(tid == *idle_iter)
	   		break;
		++idle_iter;
	}    

	mutex.mutex_lock();
	idle_tid.erase(idle_iter);
	busy_tid.push_back(tid);

	--idle;
	mutex.mutex_unlock();
	return 0;
}



#endif
