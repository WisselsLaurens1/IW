#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "util.h"

struct alias_list *aliases = NULL;


/*
 * Pre-process one given line of the input file by writing the desired output
 * to stdout (use printf), and any errors to stderr (use pr_error).
 *
 * TODO Current dummy implementation only handles whitespace.
 * Your job is to implement the desired preprocessing functionality, as
 * described in the assignment PDF.
 *
 */

char *removeLineComment(char *line){
   // ret is een pointer naar de positie van het eerste voorkomen "//"
   char *ret;
   ret = strstr(line, "//");

   // als ret !=0 is, dus er is commentaar
   if(ret != 0){
     // als ret gelijk is aan line dan betekend dit dat heel de lijn commentaar is ga naar 1.
     // als dit niet gelijk is betekend dit dat de commentaar in het midden begint 2.
     if(strcmp(ret,line)!=0){
       // 2. splits line op in tokens, de eerste token is code
       // de tweede token is de commentaar die we willen verwijderen
       char *token = strtok(line, "//");
       // voeg "\n" toe aan einde token zodat line eindigt op \n
       // token = realloc(token,strlen(token)+strlen("\n"));
       strcat(token,"\n");
       return token;
     // 1.
     }else{
       // verwijder commentaar door "0 byte" terug te geven
        return "\0";
     }

   }else{
     // als er geen commentaar is, geef lijn terug
     return line;
   }
 }

char *tabToSpace(char *line) {

  char *token = strtok(line, " \t");
  char *procesedLine="";
  char *s=" ";
  // printf("%lu\n", strlen(procesedLine));
  // printf("%lu\n",strlen(token) );

  while (token){
      if (token[strlen(token)-1] == '\n') {
        if(strlen(procesedLine) != 0){
          procesedLine = realloc(procesedLine, strlen(procesedLine)+strlen(token));
          strcat(procesedLine,token);
        }
          else{
            procesedLine = malloc(strlen(token));
            strcpy(procesedLine,token);
          }
      }
      else{
        if(strlen(procesedLine) != 0){
          procesedLine = realloc(procesedLine, strlen(procesedLine)+strlen(token)+strlen(s));
          strcat(procesedLine,token);
          strcat(procesedLine,s);
        }
          else{
            procesedLine = malloc(strlen(token)+strlen(s));
            strcpy(procesedLine,token);
            strcat(procesedLine,s);
          }

      }
      token = strtok(NULL, " \t");
  }
  return procesedLine;
}

char *removWhiteSpace(char *line){

  char *s = " \t";

  // reserveer geheugen voor kopie van line
  char *lineCopie = (char*) malloc(strlen(line)+1);
  // maak kopie van line
  strcpy(lineCopie,line);
  //splits line in tokens
  char *token = strtok(line, s);

  int difference;

  // OPGELET: We gebruiken lineCopie omdat line is aangepast door strtok

  // token is een pointer naar de eerste string na het voorkomen van s
  // line is een pointer naar het begin van line
  // als token en line niet de zelfde pointer zijn, dus niet gelijk zijn dan beteknd dit dat er wit ruimte is aan het begin van line

  if(token != line){

    // difference is het letterlijke verschil van de adressen van line en token
    // difference is een negatief getal
    difference = line-token;
    // door bij lineCopie difference aftetrekken (eigenlijk optellen: -(-difference)) verhogen we de pointer van lineCopie naar de plaats na de witruimte in het begin
    lineCopie = lineCopie-difference;
    return lineCopie;
  }else{
    return lineCopie;
  }

}

char *warning(char* line){
  char *s = "#warning";
  char *token = strstr(line, s);
  if(token == NULL){
    return line;
  }else{
    // TO DO : print warning stderr
    pr_error(token+strlen(s)+1);
    return "\0";
  }
}


void process_line(char *line){

  line = removWhiteSpace(line);
  line = tabToSpace(line);
  line = removeLineComment(line);
  // line = warning(line);
  printf("%s\n",line);


}



/*
 * Process a single-char "-C" command line option.
 *
 * TODO add your own single-char options here
 *  (e.g., "-v" for printing author and version information)
 */
void option(char c)
{
    switch (c)
    {
        case 'h':
            printf("cpp-iw: The awesome IW C preprocessor\n");
            printf("\nUsage: cpp-iw [-h -e] file.c\n");
            printf("\nRecognized options:\n");
            printf("-h\tdisplay this help message\n");
            printf("-e\tterminate preprocessing on #error\n");
            exit(0);
            break;
        default:
            pr_error("Unrecognized option '%c'\n", c);
            exit(1);
    }
}

int main(int argc, char *argv[])
{
    int i;
    char *s;

    /* Process command line options */
	for (i = 1; i < argc && *argv[i] == '-'; i++)
        option(*(argv[i]+1));

    if (argv[i] == NULL)
    {
        pr_error("No input C file provided\n");
        return 1;
    }

    /* Open the input file */
    if ( infile_open(argv[i]) )
    {
        pr_error("Could not open input file '%s'\n", argv[i]);
        return 1;
    }

    /* First pass the list test suite (make sure to add your own tests!) */
	if (alias_list_test() == 0)
        return 1;

    /* Do the actual preprocessing work line by line */
    aliases = alias_list_create();

    while ((s = infile_getline()))
    {
        process_line(s);
    }

    infile_close();
	return 0;
}
