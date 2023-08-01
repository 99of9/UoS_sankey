#include <stdio.h>
#include <stdlib.h>

#define MAXSUBJECTS 200
#define MAXNAME 20

int subjects=168;
int session[MAXSUBJECTS];
int index_list[MAXSUBJECTS];
char names[MAXSUBJECTS][MAXNAME];
int crosstable[MAXSUBJECTS][MAXSUBJECTS];

void initialize() {
  int s;
  for (s=0; s<subjects; s++) {
    session[s] = 0;
  }
}

void get_line(FILE *fp) {
  char ch=' ';
  while (ch!='\n') ch=fgetc(fp);
}

void read_crosstable() {
  int i,t,s,idx=0;
  FILE *fp;
  char ch=' ';
  fp = fopen("crosstable.csv", "r");
  ch=' ';
  while (ch!=',') ch=fgetc(fp);
  ch=' ';
  while (ch!=',') ch=fgetc(fp);
  for (s=0; s<subjects; s++) {
    fscanf(fp, "%d", &session[s]);
    printf("session %d: %d\n", s, session[s]);
    index_list[s]=idx;
    if (session[s]!=0) {
      idx++;
    }
    ch=fgetc(fp); /*remove comma or EOLN */
  }

  get_line(fp);
  get_line(fp);
  ch=' ';
  while (ch!=',') ch=fgetc(fp);
  ch=' ';
  while (ch!=',') ch=fgetc(fp);
  
  for (s=0; s<subjects; s++) {
    ch=fgetc(fp);
    i=0;
    while ((ch!=',')&&(ch!='\n')) {
      names[s][i] = ch;
      ch=fgetc(fp);
      i++;
    }  
    names[s][i] = '\0';
    //printf("names %d %s\n", s, names[s]);
  }
  get_line(fp);

  for (s=0; s<subjects; s++) {
    ch=' ';
    while (ch!=',') ch=fgetc(fp);
    ch=' ';
    while (ch!=',') ch=fgetc(fp);
    
    for (t=0; t<subjects; t++) {
      fscanf(fp, "%d", &crosstable[s][t]);
      if (t==s) {
	printf("#%d i=%d %s session %d: ", s, index_list[s], names[s], session[s]);
	printf("entry[%d][%d] %d\n", s, t, crosstable[s][t]);
      }
      fflush(NULL);
      ch=fgetc(fp); /*remove comma */
    }
    get_line(fp);
  }
  fclose(fp);
}

void write_json() {
  FILE *fp;
  int s, t;
  int first=1;
  fp = fopen("chemistry_crosstable.json", "w");
  fprintf(fp, "{\"nodes\":[\n");
  for (s=0; s<subjects; s++) {
    if (session[s]!=0) {
      if (first==1) {
	first=0;
      } else {
	fprintf(fp,",\n");
      }
      fprintf(fp,"{\"name\":\"%s (%d)\"}",names[s], crosstable[s][s]);
    }
  }
  fprintf(fp,"\n],\n");
  fprintf(fp,"\"links\":[\n");
  first=1;
  for (s=0; s<subjects; s++) {
    for (t=0; t<subjects; t++) {
      if ((session[s]!=0)&&(session[t]!=0)&&(session[t]==session[s]+1)&&((crosstable[s][t]!=0)||(session[t]==6))) {
	if (first==1) {
	  first=0;
	} else {
	  fprintf(fp,",\n");
	}
	fprintf(fp,"{\"source\":%d,\"target\":%d,\"value\":%d}", index_list[s], index_list[t], crosstable[s][t]);
      }
    }
  }
  fprintf(fp,"\n]}\n");  
  fclose(fp);
}

int main() {
  initialize();
  read_crosstable();
  write_json();
}
