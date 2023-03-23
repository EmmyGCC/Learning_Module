# Configuration Steps

1.  **Project->Manage->Run-Time Environment**
2.  find "Compiler", select "Event Recorder",make sure Variant list is "DAP"
3.  expand "I/O", select "STDOUT",select "EVR" in Variant list
4.  add code below in main.c:

```c
#include <RTE_Components.h>
#if defined(RTE_Compiler_EventRecorder)
#include <EventRecorder.h>
#endif

void main(void)
{
    ...
#if defined(RTE_Compiler_EventRecorder) \
 && defined(RTE_Compiler_IO_STDOUT_EVR)
    EventRecorderInitialize(0, 1);
#endif
    
    printf("Hello World\r\n");
    ...
}


```

5.  **View->Serial Windows->Debug (printf) View,see below**

![image-20230323113318661](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230323113318661.png)

----



EventRecorder put data in ".bss.noinit" section，if command windows shows:

>   Warning: Event Recorder not located in uninitialized memory!

1.  open "**Options for Target**" - Linker

unselect "Use Memory Layout from Target Dialog"

2.  copy Scatter File to another Folder,select this scatter file in Linker optional "Edit.."

3.  add code below right after **RW_IRAM1** section:

```c
LR_IROM1 ... {
    ....
    RW_IRAM1 ... {
    }
    ZI_RAM_UNINIT +0 UNINIT {
        .ANY (.bss.noinit)
    }
}
```

4.  save it,Compiler Project,then Debug device