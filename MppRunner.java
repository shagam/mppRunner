/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package mpprunner;

import java.lang.*;
import java.util.TreeMap;
import java.util.Random;



class SafeTree {
    TreeMap <Integer, Integer> s_tree = new TreeMap<>();
    
    public synchronized void put (int key, int val) {
        s_tree.put(key, val);
    }
    
    public synchronized Integer get (int key) {
        return s_tree.get(key);
    }   

    public Integer get_notSynchronized (int key) {
        return s_tree.get(key);
    }  

}



public class MppRunner extends java.lang.Thread {
    
    static int s_threadCnt;
    static int s_loops = 0;
    static boolean s_verbose;
    static boolean s_loose;
    static boolean s_treeMapTest;
    static SafeTree s_safeTree = new SafeTree();
    static volatile int s_sharedCounter = 0;
    Random m_randomizer = new Random();  
    
    int m_id;
    int myCounter = 0;       

    long curTime;
    long curTime2;
    
    
    public MppRunner (int id){
        m_id = id;

    }
    
    int getCounter () {
        return s_sharedCounter;
    }
    
    void setCounter (int ctr) {
        s_sharedCounter = ctr;
    }
    
    static synchronized void incCounter () {
        s_sharedCounter ++;
    } 
    
    public static void main(String[] args) /*throws InterruptedException*/ {
            //System.out.println("Hello now");
        long curTime = System.currentTimeMillis();

        s_threadCnt = Args.getInteger("threads", args, "number of concurrent threads");
        if (s_threadCnt == Integer.MAX_VALUE)
            s_threadCnt = 32; // Runtime.getRuntime().availableProcessors();

        s_loops = Args.getInteger("loops", args, "number of loops for each thread");
        if (s_loops == Integer.MAX_VALUE)
            s_loops = 1000000;

        s_verbose = Args.getBool("verbose", args, "print thread start/stop");
        
        s_loose = Args.getBool("loose", args, "without lock (Wrong result)");        

        s_treeMapTest = Args.getBool("tree", args, "tree map");
        
        System.out.format("\ntreads=%d loops=%d treeMap=%s\n", s_threadCnt, s_loops, s_treeMapTest);
                
        if (s_treeMapTest) {
            Random m_randomizer = new Random();            
            for (int n = 0; n < s_loops; n++) {
                int in = m_randomizer.nextInt(s_loops);
                s_safeTree.put(in, in);
            }                
        }
        
        Args.showAndVerify (true);                    

        MppRunner [] SharedThreads = new MppRunner[s_threadCnt];
        for (int i = 0; i < s_threadCnt; i++){
//			SharedThreads[i] = new SharedCounterThreads(1000000);
                SharedThreads[i] = new MppRunner (i);//20);
                if (s_verbose)
                System.out.format("\nspawned id=%d", i);
                SharedThreads[i].start();
        }
        for (int i = 0; i < s_threadCnt; i++){
            try {
                SharedThreads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        long endtime = System.currentTimeMillis();
        System.out.format("\ndelay=%d sharedCounter=%d\n", endtime - curTime, s_sharedCounter);
		

    }

    @Override
    public void run(){
        curTime = System.currentTimeMillis();

        if (s_treeMapTest) {
            for (int i = 0; i < s_loops ;i++){
                int in = m_randomizer.nextInt() % s_loops;
                if (in < 0)
                    in = 0 - in;
                s_safeTree.get(in);
            } 
        }
        else
        if (s_loose) {
            for (int i = 0; i < s_loops ;i++)
                s_sharedCounter ++;
        }
        
        else
        for (int i = 0; i < s_loops ;i++){
            incCounter();
//            synchronized (this) {
//                s_sharedCounter ++;
//                myCounter = getCounter();
//                myCounter += 1;
//                setCounter(myCounter);
//            }
        }
        curTime2 = System.currentTimeMillis();
        long exectime = curTime2 - curTime;
        if (s_verbose)
            System.out.format("\nfinished id=%d delay=%d counter=%d", m_id, exectime, getCounter() );		
    }
    
}
