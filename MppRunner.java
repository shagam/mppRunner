/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package mpprunner;

import java.lang.*;






public class MppRunner extends java.lang.Thread {
    
    static int s_threadCnt = 0;
    static int s_iterations;
    static boolean s_verbose;
    static volatile int s_sharedCounter = 0;
    
    int m_id;
    int myCounter = 0;       

    long curTime;
    long curTime2;
    
    
    //static 
    /**
     * @param args
     * @throws InterruptedException 
     */
    
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

        s_iterations = Args.getInteger("iterations", args, "number of loops for each thread");
        if (s_iterations == Integer.MAX_VALUE)
            s_iterations = 1000000;

        s_verbose = Args.getBool("verbose", args, "");
        
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
        for (int i = 0; i < s_iterations ;i++){

            synchronized (this) {
            incCounter();
//                SharedCounter ++;
//                myCounter = getCounter();
//                myCounter += 1;
//                setCounter(myCounter);
            }
        }
        curTime2 = System.currentTimeMillis();
        long exectime = curTime2 - curTime;
        if (s_verbose)
            System.out.format("\nfinished id=%d delay=%d counter=%d", m_id, exectime, getCounter() );		
    }
    
}
