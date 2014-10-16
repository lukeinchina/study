unsigned long long GetCPUTickCount(){ 
    unsigned long high32=0, low32=0;
#ifdef WIN32 // WIN32
    _asm
    { 
        RDTSC;
        mov high32,ebx;  
        mov low32,eax;  
    } 
#else
    __asm__ ("RDTSC" : "=a"(low32),"=d"(high32));
#endif
    unsigned longlong counter = high32;
    counter = (counter<<32) + low32;
    return counter;
}

inline __int64 RDTSC()   
{   
    __int64 TimeStamp;   

    unsigned long highDword;   
    unsigned long lowDword;   

    __asm   
    {   
        rdtsc;   

        //也可以如下这样直接嵌入机器码   
        //        //_emit 0x0F;   
        //                //_emit 0x31;   

        mov highDword,ebx;   
        mov lowDword,eax;   
    }   

    TimeStamp = highDword;   

    TimeStamp <<= 32;   
    TimeStamp |= lowDword;   

    return TimeStamp;   
}
