#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>
#include <pthread.h>
#include <time.h>
#include<unistd.h>
#include "args.h"
#include<string.h>
#include <map>
#include <limits.h>



//* Jewel Store Copyright (C) 2011-2014  TechoPhil Ltd

using namespace std;
const int THREAD_MAX = 100;

long getTimeMili () {
    timeval time;
    gettimeofday(&time, NULL);
    long millis = (time.tv_sec * 1000.0) + (time.tv_usec / 1000.0) + 0.5;
    return millis;
}

static long s_grossLoops;


static int s_threadCnt;
static int s_iteration;
static int s_loose;
static int s_hwarLock;
static int s_treeMapTest;
static int s_size;
static int s_verbose;

pthread_t               thread_id[THREAD_MAX];

class SafeCount {
    
    pthread_mutex_t   m_mutex;
    
    public:

    SafeCount () {
        m_mutex = PTHREAD_MUTEX_INITIALIZER;        
        int stat = pthread_mutex_init (& m_mutex, NULL);        
    }
    
    void inc (int val) {
        int stat = pthread_mutex_lock (&m_mutex);
        if (stat)
            exit(-3);
        s_grossLoops += val;    
        int stat1 = pthread_mutex_unlock (&m_mutex);         
        if (stat1)
            exit(-4);
    }
};

static SafeCount * safeCount = new SafeCount();

inline long InterlockedIncrement(long* p, int delta)
{
    return __atomic_add_fetch(p, delta, __ATOMIC_SEQ_CST);
    
//      inline int fetch_and_add( int * variable, int value ) {
//      asm volatile("lock; xaddl %%eax, %2;"
//                   :"=a" (value)                  //Output
//                   :"a" (value), "m" (*variable)  //Input
//                   :"memory");
//      return value;
//  }
}

class SafeTree {
    
    pthread_mutex_t   m_mutex;
    map <int, int> m_tree;

public:    
    SafeTree () {
        m_mutex = PTHREAD_MUTEX_INITIALIZER;        
        int stat = pthread_mutex_init (& m_mutex, NULL);        
    }
    void put (int key, int val) {

        if (s_threadCnt > 1) {
            int stat = pthread_mutex_lock (&m_mutex);
            if (stat)
                exit(-3);
        }
        
        m_tree.insert (make_pair(key, val));        
        if (s_threadCnt > 1) {
            int stat1 = pthread_mutex_unlock (&m_mutex);         
            if (stat1)
                exit(-4);
        }
    }
    
    int get (int key) {
        int val = -1;
        map<int,int>::iterator it;        
        if (s_threadCnt > 1) {
            int stat = pthread_mutex_lock (&m_mutex);
            if (stat)
                exit(-3);
        }
        it = m_tree.find(key);
        if (s_threadCnt > 1) {
            int stat1 = pthread_mutex_unlock (&m_mutex);         
            if (stat1)
                exit(-4);
        }
        if( it != m_tree.end())
            val = it->second;
        else
            val = INT_MAX;
        return val;
    }
};

static SafeTree * s_safeTree = new SafeTree();

void *thread ( void *ptr ) {
  size_t tmp = (size_t) ptr;
  int id = (int) tmp;
  if (s_verbose)
    fprintf (stderr, "\nthread created=%d", id);
  if (s_treeMapTest) {
        for (long n = 0;  n < s_iteration; n++)
            s_safeTree->get (rand());
  }
  else
  for (long n = 0;  n < s_iteration; n++) {
      if (s_loose)
          s_grossLoops ++;
      else if (s_hwarLock)
          InterlockedIncrement(& s_grossLoops, 1);
      else // default
          safeCount->inc(1);


      
  }
  if (s_verbose)
    fprintf (stderr, "\nthread finish=%d", id);
  return NULL;
}

int main(int argc, char * argv[])  {
    //InterlockedIncrement(& s_grossLoops, 1);
    //SafeCount safeCount = new SafeCount;
    s_iteration = getInteger ("loops", argc, argv, "iterations");
    if (s_iteration == -1)
        s_iteration = 1000000;
    
    s_threadCnt = getInteger ("thread", argc, argv, "concurrent thread count");
    if (s_threadCnt == -1)
        s_threadCnt = 32; //getCPUCount();

    s_loose = getBool("loose", argc, argv, "no lock; default mutex");
    s_hwarLock = getBool("atomicHwar_", argc, argv, "__atomic_add_fetch");
    
    s_verbose  =  getBool("verbose", argc, argv, "printout: thread_create, thread_end");    

    s_treeMapTest = getBool("tree", argc, argv, "map_get");
    s_size = getBool("size", argc, argv, "number of element");
    if (s_size == -1)
        s_size = 100000;
    if (s_treeMapTest) {
        for (int n = 0; n < s_size; n++) {
            int key = rand() % s_size;
            s_safeTree->put (key, key);
        }
    }
    
    args_report();
    const char * err = verify (argc, argv);
    if (err != NULL) {
        fprintf (stderr, "invalid param=%s  \n", err);
        exit (3);
    }
    long  startMili = getTimeMili();     
    for (int n = 0; n < s_threadCnt; n++) {
        size_t tmp = n;
        int stat = pthread_create( &thread_id[n], NULL, thread, (void*) tmp);
        if (stat) {
            exit(-2);
        }
    }

    
    for (int n = 0; n < s_threadCnt; n++) {
        pthread_join( thread_id[n], NULL);
    }
     
    long endMili = getTimeMili();
    long delay = endMili - startMili;
    
    fprintf (stderr, "\ndelay=%ld sharedCount=%ld\n", delay, s_grossLoops);
    //exit (0);
             
    return 0;
}

