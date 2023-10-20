#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <X11/Xlib.h>

#define len 2048
int last_index;
int in_background;
int is_pipe;
int len_command;

struct command{
    char** argv;
};

struct vector{
    int number[10];
};


struct vector Vector1,Vector2,result;;
struct thread_vector{
    struct vector v1,v2;
    int size;
    int start;
    int end;
    int id;
    struct vector result;
};

//this tokenise input from user by space
void split(char* line,char** argv){
    char* token=strtok(line," ");
    int index=0;
    while(token!=NULL){
        argv[index]=token;
        index++;
        //printf("%s:",token);
        token=strtok(NULL," ");
    }
    argv[index]=NULL;
    last_index=index;
    return ;
}

void addition(void *arg){
    struct thread_vector* thread = (struct thread_vector*)arg;
    for(int i=thread->start;i<=thread->end;i++){
        result.number[i]=thread->v1.number[i]+thread->v2.number[i];
        //printf("%d ",result.number[i]);
    }
}

void substraction(void *arg){
    struct thread_vector* thread = (struct thread_vector*)arg;
    for(int i=thread->start;i<=thread->end;i++){
        result.number[i]=thread->v1.number[i]-thread->v2.number[i];
        //printf("%d ",result.number[i]);
    }
}

void dot_product(void *arg){
    struct thread_vector* thread = (struct thread_vector*)arg;
    for(int i=thread->start;i<=thread->end;i++){
        result.number[0]+=thread->v1.number[i]*thread->v2.number[i];
        //printf("%d ",result.number[i]);
    }
}



void thread(char** argv){
	int code=0;

    FILE* fp1 = fopen(argv[1],"r");
    FILE* fp2 = fopen(argv[2],"r");
    if(fp1==NULL || fp2==NULL){ printf("null"); }
    struct vector v1,v2;
    int index1=0;
    while (index1 < 10000 && fscanf(fp1, "%d", &v1.number[index1]) == 1) {
        index1++;


    }
    int index2=0;
    while (index2 < 10000 && fscanf(fp2, "%d", &v2.number[index2]) == 1) {
        index2++;
    }
    if(index2!=index2){printf("wrong size"); return ;}


    int index=index1;


    //printf("hello");


    fclose(fp1);
    fclose(fp2);

    int n=atoi(argv[3]);
    struct thread_vector array[n];
    int interval=index/n;
    for(int i=0;i<n;i++){
        array[i].v1=v1;
        array[i].v2=v2;
        array[i].size=index;
        array[i].start=i*interval;
        array[i].end=(i+1)*interval-1;
        //printf("%d %d \n",array[i].start,array[i].end);
        array[i].id=i;
    }
    pthread_t threads[n];
    array[n-1].end=index-1;

    if(strcmp(argv[0],"addvec")==0){
        for(int i=0;i<n;i++){
        	code=1;
            pthread_create(&threads[i], NULL, addition, &array[i]);
        }
    }else if(strcmp(argv[0],"subvec")==0){
        for(int i=0;i<n;i++){
        code=1;
            pthread_create(&threads[i], NULL, substraction, &array[i]);
        }
    }else if(strcmp(argv[0],"dotprod")==0){
        for(int i=0;i<n;i++){
            pthread_create(&threads[i], NULL, dot_product, &array[i]);
        }
    }else{
        printf("wlse");
        return;
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }
    
    if(code){
    for(int i=0;i<index;i++){

            printf("%d ",result.number[i]);

    }
    }else{
    	printf("%d ",result.number[0]);
    }



}

void execute(char** argv){
    pid_t pid;
    int status;

    if(strcmp(argv[0],"addvec")==0 || strcmp(argv[0],"subvec")==0 || strcmp(argv[0],"dotprod")==0){
        printf("thread");
        thread(argv);
        return;

    }
    if(strcmp(argv[0],"vi")==0){

        char* temp_array[2];
        temp_array[0]="./text_editor";
        temp_array[1]=argv[1];
        printf("%s",temp_array[1]);
        pid=fork();
        if(pid==0){
            execlp("./text_editor","text_editor",argv[1],NULL);
            exit(1);

        }else{
            while(wait(&status)!=pid){
            }
        }
        return ;
    }
    if(strcmp(argv[0],"cd")==0){
            char str[len];
            int index=2;
            strcpy(str,argv[1]);
            while(argv[index]!=NULL){
                strcat(str,argv[index]);
                index++;
            }
            chdir(str);
            return ;
        }
    //remaining command
    pid=fork();
    if(pid==0){
        execvp(*argv,argv);
        exit(1);
    }else if(pid>0){
        //if & is given don't wait for child
        if(in_background==1){return ;}
        //waiting for child
        while(wait(&status)!=pid){
        }
    }
    return ;
}
int split_command(char* line,struct command *lines){
    char* argv[100];
    char* token=strtok(line,"|");
    int index=0;
    while(token!=NULL){
        argv[index]=token;

        index++;
        //printf("%s:",token);
        token=strtok(NULL,"|");

    }
    argv[index]=NULL;
    for(int i=0;i<index;i++){
        lines[i].argv=(char**)malloc(100*sizeof(char*));
        split(argv[i],lines[i].argv);
        //printf("%s",argv[i]);
    }


    return index;
}

void check_pipe(char* line){
    int index=0;
    while(line[index]!='\0'){
        if(line[index]=='|'){
            is_pipe=1;
            return ;
        }
        index++;
    }

}

void n_pipe(char* line){
    struct command lines[100];
    int len_com = split_command(line,lines);
    len_command=len_com;



    int child=fork();
    int c_status;
    if(child==0){
        int fd[2];//1 write 0 read
        int prev_pipe=STDIN_FILENO;
        for(int i=0;i<len_command-1;i++){
            if(pipe(fd)==-1){
                printf("Something Wrong with pipe");
            }
            int id=fork();
            if(id==0){
                if(prev_pipe!=STDIN_FILENO){
                    dup2(prev_pipe,STDIN_FILENO);
                    close(prev_pipe);
                }
                dup2(fd[1],STDOUT_FILENO);
                close(fd[1]);
                execvp(lines[i].argv[0],lines[i].argv);
                exit(1);

            }
            close(prev_pipe);
            close(fd[1]);
            prev_pipe=fd[0];
        }



            if(prev_pipe!=STDIN_FILENO){
                    dup2(prev_pipe,STDIN_FILENO);
                    close(prev_pipe);
            }
            execvp(lines[len_com-1].argv[0],lines[len_com-1].argv);

        close(fd[1]);
        close(fd[0]);
    }else{
        while(wait(&c_status)!=child){
            }
    }
}

int main(){




    while(1){

        char line[1000];
        line[0]='\0';
        char* prev=NULL;
        is_pipe=0;
        in_background=0;
        char *argv[100];
        //empty input
        prev = readline("Existence>>");
         printf("running");
        if(prev[0]=='\0'){
            break;
        }
        //multiline functionality
        printf("running");
        while(prev[strlen(prev)-1]=='\\'){
            prev[strlen(prev)-1]='\0';
            strcat(line,prev);
            prev = readline("");
        }
        strcat(line,prev);
        //printf("%s",line);






        add_history(line);

        size_t l=strlen(line);
        //line[l-1]='\0';//making last next line as NULL
        check_pipe(line);
        if(strcmp(line,"exit")==0){break;}
        if(is_pipe==0){
            split(line,argv);//spliting by " "
            if(line[l-1]=='&'){in_background=1;line[l-1]='\0';}

            execute(argv);//excuting the line
        }else{
            n_pipe(line);
        }

    }
    return 0;

}
