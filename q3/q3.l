%{
    #include <stdio.h>
    #include <stdlib.h>

    #define MAXCUSTOMERS 50
    #define MAXCUSTOMERNAMELENGTH 50
    #define MAXVALSPERCUSTOMER 50

    char customers[MAXCUSTOMERS][MAXCUSTOMERNAMELENGTH];
    int custSize= 0;

    int dds[MAXCUSTOMERS][MAXVALSPERCUSTOMER];
    int mms[MAXCUSTOMERS][MAXVALSPERCUSTOMER];
    int vals[MAXCUSTOMERS][MAXVALSPERCUSTOMER];
    int valsSize[MAXCUSTOMERS];

    int state = 0;
    int custInd = 0;
    int valInd;
%}

%%
"//".* {
    //fprintf(yyout, "\nComment");
}

$(" ")*[A-Z]+[0-9]+ {
    if (state == 0 || state == 3 || state == -1) {
        state = 1;
    } else {
        fprintf(yyout, "0\n");
        exit(0);
    }

    int len = strlen(yytext);
    int start = 0;
    for (int i = 0; i < len; i++) {
        if (isalpha(yytext[i])) {
            len = len - i;
            start = i;
            break;
        }
    }

    char temp[MAXCUSTOMERNAMELENGTH];
    strncpy(temp, &yytext[start], len);
    temp[len] = '\0';

    int tempInd = -1;
    for (int i = 0; i < custSize; i++) {
        if(strcmp(temp, customers[i]) == 0) {
            tempInd = i;
            break;
        }
    }

    if (tempInd == -1) {
        strcpy(customers[custSize], temp);
        tempInd = custSize;
        valsSize[custSize] = 0;
        custSize++;
    }

    custInd = tempInd;
    valInd = valsSize[custInd];

    //fprintf(yyout, "\ncust is %s. ", temp); 

};

[0-9]+"/"[0-9]+ {
    if (state == 1 || state == 3) {
        state = 2;
    } else {
        //ignoring
        state = -1;
        continue;
    }


    int d = (yytext[1] - '0') + 10 * (yytext[0] - '0');
    int m = (yytext[4] - '0') + 10 * (yytext[3] - '0');


    if (d > 30 || m > 12) {
        fprintf(yyout, "0\n");
        exit(0);
    }
    dds[custInd][valInd] = d;
    mms[custInd][valInd] = m;
    //fprintf(yyout, "%d %d. ", d, m);
};

[0-9]+";" {
    if (state == 2) {
        state = 3;
    } else {
        //ignoring
        state = -1;
        continue;
    }
    int temp = 0;
    int len = strlen(yytext);
    len -= 2; //last is ";';
    int pow = 1;
    while(len >= 0) {
        temp = temp + pow * (yytext[len] - '0');
        pow *= 10;
        len--;
    }
    valInd = valsSize[custInd];
    valsSize[custInd]++;
    vals[custInd][valInd] = temp;
    //fprintf(yyout, "%d ", temp);
}

. ;

\n {
    state = 0;
}

%%
int yywrap() { 
    return 1;
}

int main() {
    yyin = fopen("data.txt", "r");
    yyout = fopen("output.txt", "w");
    yylex();

    FILE* inFile = fopen("input.txt", "r");
    char data[10];
    fscanf(inFile, "%s", &data);

    int d = (data[1] - '0') + 10 * (data[0] - '0');
    int m = (data[4] - '0') + 10 * (data[3] - '0');


    if (d > 30 || m > 12) {
        fprintf(yyout, "0\n");
        exit(0);
    }
    fprintf(yyout, "%d %d\n", d, m);
    fprintf(yyout, "customers %d\n", custSize);
    for (int i = 0; i < custSize; i++) {
        fprintf(yyout, "%s has %d vals\n", customers[i], valsSize[i]);
    }
}