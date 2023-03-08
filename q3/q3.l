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
};

"$" {
    if (state == 0 || state == 5 || state == -1)
        state = 1;
    else 
        state = -1;
};

[A-Z]+[0-9]+ {
    if (state == 1) {
        state = 2;
    } else {
        //ignoring
        state = -1;
        continue;
    }

    char temp[MAXCUSTOMERNAMELENGTH];
    strcpy(temp, yytext);

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
    if (state == 2 || state == 5) {
        state = 3;
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

    valInd = valsSize[custInd];
    dds[custInd][valInd] = d;
    mms[custInd][valInd] = m;
    //fprintf(yyout, "%s %d %d %d\n", customers[custInd], d, m, valInd);
};

[0-9]+ {
    if (state == 3) {
        state = 4;
    } else {
        //ignoring
        state = -1;
        continue;
    }
    int temp = atoi(yytext);
    valInd = valsSize[custInd];
    vals[custInd][valInd] = temp;
    //fprintf(yyout, "%s %d %d\n", customers[custInd], temp, valInd);
    valsSize[custInd]++;
};

";" {
    state = 5;
};

. ;

\n {
    state = 0;
};

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

    int numvalid = 0;
    int mxVal = -1;
    int mxCust = -1;

    //fprintf(yyout, "customers %d\n", custSize);
    for (int i = 0; i < custSize; i++) {
        //fprintf(yyout, "%s has %d vals\n", customers[i], valsSize[i]);
        for (int j = 0; j < valsSize[i]; j++) {
            //fprintf(yyout, "{ %d/%d %d } ", dds[i][j], mms[i][j], vals[i][j]);
            if (dds[i][j] == d && mms[i][j] == m) {
                numvalid++;
                if (vals[i][j] > mxVal) {
                    mxVal = vals[i][j];
                    mxCust = i;
                }
            }
        }
        //fprintf(yyout, "\n");
    }
    if (numvalid == 0) 
        fprintf(yyout, "$0$#");
    else 
        fprintf(yyout, "$%d$%s#", numvalid, customers[mxCust]);
}
