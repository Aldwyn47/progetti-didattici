static struct Caverna *Dept_Charge (struct Caverna*Node){
    while (NextStep(Node)!=Node){
        struct Caverna *follower = Node;
        int flag = -1;
        struct Caverna *scan = NULL;
        if (Node->NextSinistra!=NULL){
            scan = Node->NextSinistra;
            flag = 0;
        } else if (Node->NextAvanti!=NULL){
            scan = Node->NextAvanti;
            flag = 1;
        } else if (Node->NextDestra!=NULL){
            scan = Node->NextDestra;
            flag = 2;
        }
        while (NextStep(scan)!=scan){
            follower = scan;
            if (scan->NextSinistra!=NULL){
                scan = scan->NextSinistra;
                flag = 0;
            } else if (scan->NextAvanti!=NULL){
                scan = scan->NextAvanti;
                flag = 1;
            } else if (scan->NextDestra!=NULL){
                scan = scan->NextDestra;
                flag = 2;
            }
        }
        if (scan!=NULL){
            printf("Dept charge distruggerà ora il nodo %p\n", scan);
            free(scan);
            if (flag==0)
                follower->NextSinistra = NULL;
            if (flag==1)
                follower->NextAvanti = NULL;
            if (flag==2)
                follower->NextDestra = NULL;
        }
    }
    printf("Dept charge distruggerà ora il nodo madre.\n");
    free(Node);
    return NULL;
}