#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#define max_length 10000

char* FILE_NAME;

void save(char** screen, int row, int col) {
    FILE* fp = fopen(FILE_NAME, "w");
    if (fp == NULL) {
        printf("Failed to open the file for writing.\n");
        return;
    }
    int line=0;
    for (int c_r = 0; c_r < row; c_r++) {
        line++;
        if(screen[c_r][0]==127){break;}
        int c_c = 0;
        while (screen[c_r][c_c] != '\0' && screen[c_r][c_c]!=127) {
            fputc(screen[c_r][c_c], fp);
            c_c++;
        }

        fputc('\n', fp); // Add newline after each row
    }
    printf("line %d ++",line);

    fclose(fp);
}









void editor(char* message){
    int start_row=0,start_col=0;
    int rows = LINES;  // Get the number of rows
    int cols = COLS;
    int more_text=0;
    int cur_rows=0;
    int cur_col=0;
    int cursor_row=0;
    int cursor_col=0;
    int ky=0;
    int first=1;
    //printw("%d %d",rows,cols);


    char** screen;
    char* copy_text;
    char* original_text;
    screen=(char**)malloc(rows*sizeof(char*));
    if(screen==NULL){printf("wrong");return ;}
    copy_text=(char*)malloc(max_length*sizeof(char));
    if(copy_text==NULL){printf("wrong");return ;}
    original_text=(char*)malloc(max_length*sizeof(char));
    if(original_text==NULL){printf("wrong");return ;}
    strcpy(original_text,message);
    strcpy(copy_text,message);
    for(int i=0;i<cols;i++){
        screen[i]=(char*)malloc(cols*sizeof(char));
        if(screen[i]==NULL){printf("wrong");return ;}
    }

    //printw("hello");
    int to_exit=0;
    for(int i=0;i<rows;i++){
            screen[i][0]=127;
            screen[i][1]='\0';
        }
    while(!to_exit){
        //printw("%s",message);

        char* temporaty_text;
        strcpy(temporaty_text,message);
        cur_rows=0;
        cur_col=0;
        int text_index=0;
        //printw("%s",temporaty_text);
        if(first){
            while( temporaty_text[text_index]!='\0' && (screen[rows-1][1]=='\0'||screen[rows-1][0]==127)){
                    //printw("%d",c_rows);
                    int i=0;
                    for(i=0;i<cols;i++){
                        if(temporaty_text[text_index]=='\0'||temporaty_text[text_index]=='\n'){break;}
                        screen[cur_rows][i]=temporaty_text[text_index];
                        text_index++;
                    }
                    //printw("%s\n",screen[cur_rows]);
                    if(temporaty_text[text_index]=='\n'){text_index++;}
                    if(temporaty_text[text_index]=='\0'){break;}

                    cur_rows++;

            }
        }
        if(cur_rows==text_index && text_index<strlen(temporaty_text)){more_text=1;}
        cur_rows=0;
        while(screen[cur_rows][0]!=127){
            cur_col=0;
            while(screen[cur_rows][cur_col]!='\0'){
                move(cur_rows,cur_col);
                printw("%c",screen[cur_rows][cur_col]);
                cur_col++;
            }
            cur_rows++;
        }
        if(first){
            cursor_row=cur_rows-1;
            cursor_col=cur_col;
        }
        move(cursor_row,cursor_col);
        first=0;
        ky=getch();
        //printf("%d",ky);
        if(isprint(ky)){
            memmove(&screen[cursor_row][cursor_col+1],&screen[cursor_row][cursor_col],cols-cursor_col);
            move(cursor_row,cursor_col);
            screen[cursor_row][cursor_col]=ky;
            cursor_col=cursor_col+1;
            //printf("%s",screen[cursor_row]);
        }else{
            switch(ky){
                case 27://esc
                    to_exit=1;
                    break;
                case 330://del
                    //memmove(&screen[cursor_row][cursor_col],&screen[cursor_row][cursor_col+1],cols-cursor_col-1);
                    int temp_r=cursor_col+1;
                    while(screen[cursor_row][temp_r]!='\0'){screen[cursor_row][temp_r-1]=screen[cursor_row][temp_r];temp_r++;}
                    screen[cursor_row][temp_r-1]='\0';



                    break;
                case KEY_LEFT:
                    if(cursor_col>0){
                        cursor_col--;
                    }
                    break;
                case KEY_DOWN:
                    if(cursor_row<(rows-1) && screen[cursor_row+1][cursor_col]!='\0'){cursor_row++;}
                    break;
                case KEY_RIGHT:
                    if(cursor_col<(cols-1) && screen[cursor_row][cursor_col]!='\0'){cursor_col++;}
                    break;
                case KEY_UP:
                    if(cursor_row>0 && screen[cursor_row-1][cursor_col]!='\0'){cursor_row--;}
                    break;
                case KEY_IC: //ctrl+s
                    save(screen, rows, cols);
                    to_exit=1;
                    break;
                case KEY_END://ctrl+x
                    to_exit=1;
                    break;
                default:
                    printf("wrong key");
            }
        }
        refresh();
        clear();


    }
}


int main(int argc,char** argv){


    char* message = (char*)malloc(max_length*sizeof(char));
    FILE_NAME=argv[1];

    if(argc !=2){
        printf("wrong format used");
        return 0;
    }

    FILE* fp;
    fp = fopen(argv[1],"r+");
    if(fp==NULL){
        printf("wrong format used");
        return 0;
    }


    int index=0;
    char c;
    while((c=fgetc(fp))!=EOF){
        message[index]=c;
        index++;
    }
    message[index]='\0';
    fclose(fp);


    nodelay(stdscr, TRUE);
    initscr();    //initialize screen for text box
    noecho();	//disable echo
    cbreak();   //get all the key presses
    keypad(stdscr,TRUE);





    editor(message);
    refresh();


    endwin();//deallocate memory
    return 0;
}
