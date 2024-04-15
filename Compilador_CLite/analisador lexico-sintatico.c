/*
NOME: OLIVER KIERAN GALVÃO MCCORMACK
*/

/*
Gramática da linguagem Clite
A sintaxe da linguagem CLite está descrita na notação EBNF, os <não-terminais> da gramática são nomes entre parênteses angulares < e > e os símbolos terminais (átomos do analisador léxico)
estão em negrito ou entre aspas (Ex: “;”). A notação { a } denotará a repetição da cadeia a zero, uma ou mais vezes (a*) e a construção [ b ] é equivalente a b|λ, ou seja, indica que a cadeia b é opcional.

<programa> ::= int main “(” void “)” “{“ <declaracoes> <comandos> “}”
<declaracoes> ::= { <declaracao> } 
<declaracao> ::= <tipo> <lista_variavel> “;” 
<tipo> ::= int | bool  
<lista_variavel> ::= identificador { “,” identificador } 
<comandos> ::= { <comando> } 
<comando> ::=  “;”               |  
  <bloco_comandos>  |  
  <atribuicao>      |  
  <comando_if>      | 
  <comando_while>   | 
  <comando_entrada> | 
  <comando_saida> 
 
<bloco_comandos> ::= “{“ <comandos> “}” 
<atribuicao> ::=  identificador “=” <expressao> “;” 
<comando_if> ::= if “(” <expressao> “)” <comando>  
[else <comando>] 
<comando_while> ::= while “(” <expressao> “)” <comando> 
<comando_entrada> ::= scanf “(“ <lista_variavel> “)” “;”
<comando_saida> ::= printf “(“ <expressao> { “,” <expressao> } “)” “;” 
<expressao> ::= <expressao_logica> { “||” <expressao_logica> } 
<expressao_logica>::= <expressao_relacional> { “&&” <expressao_relacional> } 
<expressao_relacional> ::= <expressao_adicao>  
[ <op_relacional> <expressao_adicao> ] 
<op_relacional> ::= “<” | “<=” | “==” | “!=” | “>” | “>=” 
<expressao_adicao> ::= <expressao_multi> { (“+” | “−” ) <expressao_multi> } 
<expressao_multi>::= <operando> {  ( “*” | “/” ) <operando> } 
<operando> ::= identificador | 
numero        | 
true          | 
false         | 
“(“ <expressao> “)”
*/

#include <stdio.h> // printf
#include <stdlib.h> // strtol
#include <ctype.h> // isxdigit
#include <string.h> // strncmp

//########### INICIO DEFINICOES Analisador Lexico ###################
typedef enum{ // enum com todos os estados, palavras reservadas e operadores
    ERRO,
    INTEIRO,
    MAIN,
    ABRE_PAR,
    VOID_MAIN,
    FECHA_PAR,
    ABRE_CHAVE,
    FECHA_CHAVE,
    PONTO_VIRGULA,
    BOOLEANO,
    VIRGULA,
    SE,
    SENAO,
    ENQUANTO,
    ENTRADA,
    SAIDA,
    IDENTIFICADOR,
    NUMERO,
    OP_ATRI,
    OP_OR,
    OP_AND,
    OP_MENOR,
    OP_MENOR_IGUAL,
    OP_IGUAL,
    OP_DIF,
    OP_MAIOR,
    OP_MAIOR_IGUAL,
    OP_SOMA,
    OP_SUB,
    OP_MULT,
    OP_DIV,
    VERDADEIRO,
    FALSO,
    EOS
}TAtomo;

typedef struct{ // usado para armazenar estado, linha, numero e identificador atuais
    TAtomo atomo;
    int linha;
    int atributo_numero; // numero decimal convertido de hexadecimal
    char atributo_ID[16];
}TInfoAtomo;

void leArquivo(char *filename); // funcao para ler arquivo e colocar o texto no buffer

int linha = 1;
// buffer do codigo analisado

//char *buffer = "/*\n" 
//               "programa le dois numeros\n"  
//               "inteiros e encontra o maior\n"  
//               "\n" 
//               "int main(void){\n" 
//                    "int _num1, _num2;\n" 
//                    "int _maior;\n" 
//                    "scanf(_num1);\n"
//                    "scanf(_num2);\n" 
//                    "if( _num1 > _num2 )\n"  
//                    "    _maior = _num1;\n" 
//                    "else\n" 
//                    "    _maior = _num2;\n" 
//                    "\n" 
//                    "printf(_maior); // imprime o maior valor\n" 
//                    "}\n";

char *buffer;

// definicao de funcoes lexicas
TInfoAtomo obter_atomo();
TInfoAtomo reconhece_id();
TInfoAtomo reconhece_num();

//########### FIM DEFINICOES Analisador Lexico ###################

//########### INICIO DEFINICOES  Analisador Sintatico ###################

TAtomo lookahead;
TInfoAtomo infoAtomo;
// vetor com todas as strings que representam os estados, palavras reservadas e operadores
char *strAtomo[]={"Erro Lexico","int","main","(","void",")","{","}",";","bool","virgula","if","else",
                  "while","scanf","printf","identificador","numero","=","||","&&","<","<=","==","!=",
                  ">",">=","+","-","*","/","true","false","Fim de buffer(EOS)"};

// definicao das funções dos simbolos nao-terminais

void programa(); // Simbolo inicial da gramatica
void declaracoes();
void declaracao();
void tipo();
void lista_variavel();
void comandos();
void comando();
void bloco_comandos();
void atribuicao();
void comando_if();
void comando_while();
void comando_entrada();
void comando_saida();
void expressao();
void expressao_logica();
void expressao_relacional();
void op_relacional();
void expressao_adicao();
void expressao_multi();
void operando();

// definicao da funcao consome
void consome( TAtomo atomo );

//########### FIM DEFINICOES  Analisador Sintatico ###################

int main(){
    char *iniBuffer;
    iniBuffer = buffer;

    leArquivo("entrada.txt");

    printf("Analisando:\n%s\n\n",buffer);
    
    // inicializacao de variaveis;
    infoAtomo = obter_atomo();
    lookahead = infoAtomo.atomo;
    
    // chamada da funcao de inicio da gramatica
    programa();

    printf("\n%d linhas analisadas. Programa sintaticamente correto.\n", infoAtomo.linha);

    free(iniBuffer);

}

//referencia: https://stackoverflow.com/questions/174531/how-to-read-the-content-of-a-file-to-a-string-in-c
void leArquivo(char *nome_arquivo)
{
    long length;
    FILE * arquivo = fopen (nome_arquivo, "rb");

    if (arquivo)
    {
        fseek (arquivo, 0, SEEK_END);
        length = ftell (arquivo);
        fseek (arquivo, 0, SEEK_SET);
        buffer = malloc (length);
    
        if (buffer)
        {
            fread (buffer, 1, length, arquivo);
        }
        fclose (arquivo);
    }

    else
    {
        printf("\nErro ao abrir arquivo.\n");
    }
}

//########### INICIO Analisador Lexico ###################
/*
referencia: https://cplusplus.com/reference/cstring/strncmp/

funcao lexica para obter tokens/atomos

muda o estado de enum, percorre o buffer e printa linha de cada token
*/
TInfoAtomo obter_atomo(){
    TInfoAtomo infoAtomo;
    infoAtomo.atomo=ERRO;

    

    // ignora delimitadores
    while(*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r'){
        if(*buffer=='\n')
            linha++;

        buffer++;
    }

    if(strncmp(buffer, "//",2) == 0) // pula comentario de 1 linha
    {
        printf("# %d:comentario curto\n", linha);

        while(*buffer != '\n') // vai ate encontrar final da linha
            buffer++;
        
        buffer++;linha++;
    }

    if(strncmp(buffer, "/*",2) == 0) // pula comentario de multiplas linhas
    {
        printf("# %d:comentario longo\n", linha);

        while(strncmp(buffer, "*/",2) != 0) // vai ate encontrar o marcador de final de comentario
        {
            if(*buffer == '\n')
                linha++;
            
            

            buffer++;
        }
        

        buffer++;buffer++;

        // ignora delimitadores depois de comentario longo
        while(*buffer == ' ' || *buffer == '\n' || *buffer == '\t' || *buffer == '\r')
        {
            if(*buffer=='\n')
                linha++;

            buffer++;
        }
    }
    
    if(strncmp(buffer,"int",3) == 0) // analisa palavra reservada int
    {
        buffer++;buffer++;buffer++;
        infoAtomo.atomo = INTEIRO;
        printf("# %d:int\n", linha);
    }

    else if(strncmp(buffer,"main",4) == 0) // analisa palavra reservada main
    {
        buffer++;buffer++;buffer++;buffer++;
        infoAtomo.atomo = MAIN;
        printf("# %d:main\n", linha);
    }

    else if(*buffer == '(') // analisa abertura de parentesis
    {
        buffer++;
        infoAtomo.atomo = ABRE_PAR;
        printf("# %d:abre_par\n", linha);
    }

    else if(strncmp(buffer,"void",4) == 0) // analisa palavra reservada void
    {
        buffer++;buffer++;buffer++;buffer++;
        infoAtomo.atomo = VOID_MAIN;
        printf("# %d:void\n", linha);
    }

    else if(*buffer == ')') // analisa fechamento de parentesis
    {
        buffer++;
        infoAtomo.atomo = FECHA_PAR;
        printf("# %d:fecha_par\n", linha);
    }

    else if(*buffer == '{') // analisa abertura de chave
    {
        buffer++;
        infoAtomo.atomo = ABRE_CHAVE;
        printf("# %d:abre_chave\n", linha);
    }

    else if(*buffer == '}') // analisa fechamento de chave
    {
        buffer++;
        infoAtomo.atomo = FECHA_CHAVE;
        printf("# %d:fecha_chave\n", linha);
    }

    else if(*buffer == ';') // analisa ponto e virgula
    {
        buffer++;
        infoAtomo.atomo = PONTO_VIRGULA;
        printf("# %d:ponto_virgula\n", linha);
    }

    else if(strncmp(buffer,"bool",4) == 0) // analisa palavra reservada bool
    {
        buffer++;buffer++;buffer++;buffer++;
        infoAtomo.atomo = BOOLEANO;
        printf("# %d:bool\n", linha);
    }

    else if(*buffer == '_') // analisa identificador
    {
        infoAtomo = reconhece_id(); // chamada do reconhecedor de identificador

        if(infoAtomo.atomo == ERRO)
        {
            printf("\nerro lexico: identificador invalido\n");
            exit(1);
        }

        printf("# %d:identificador | %s\n", linha, infoAtomo.atributo_ID);
    }

    else if(*buffer == ',') // analisa virgula
    {
        buffer++;
        infoAtomo.atomo = VIRGULA;
        printf("# %d:virgula\n", linha);
    }

    else if(*buffer == '=') // analisa operador de atribuicao
    {
        buffer++;
        infoAtomo.atomo = OP_ATRI;
        printf("# %d:operador atribuicao\n", linha);
    }

    else if(strncmp(buffer, "if", 2) == 0) // analisa palavra reservada if
    {
        buffer++;buffer++;
        infoAtomo.atomo = SE;
        printf("# %d:if\n", linha);
    }

    else if(strncmp(buffer, "else", 4) == 0) // analisa palavra reservada else
    {
        buffer++;buffer++;buffer++;buffer++;
        infoAtomo.atomo = SENAO;
        printf("# %d:else\n", linha);
    }

    else if(strncmp(buffer, "while", 5) == 0) // analisa palavra reservada while
    {
        buffer++;buffer++;buffer++;buffer++;buffer++;
        infoAtomo.atomo = ENQUANTO;
        printf("# %d:while\n", linha);
    }

    else if(strncmp(buffer, "scanf", 5) == 0) // analisa palavra reservada scanf
    {
        buffer++;buffer++;buffer++;buffer++;buffer++;
        infoAtomo.atomo = ENTRADA;
        printf("# %d:scanf\n", linha);
    }

    else if(strncmp(buffer, "printf", 6) == 0) // analisa palavra reservada printf
    {
        buffer++;buffer++;buffer++;buffer++;buffer++;buffer++;
        infoAtomo.atomo = SAIDA;
        printf("# %d:printf\n", linha);
    }

    else if(strncmp(buffer, "||", 2) == 0) // analisa operador OR
    {
        buffer++;buffer++;
        infoAtomo.atomo = OP_OR;
        printf("# %d:||\n", linha);
    }

    else if(strncmp(buffer, "&&", 2) == 0) // analisa operador AND
    {
        buffer++;buffer++;
        infoAtomo.atomo = OP_AND;
        printf("# %d:&&\n", linha);
    }

    else if(*buffer == '<') // analisa operador menor que
    {
        buffer++;
        infoAtomo.atomo = OP_MENOR;
        printf("# %d:<\n", linha);
    }

    else if(strncmp(buffer, "<=", 2) == 0) // analisa operador menor que ou igual a
    {
        buffer++;buffer++;
        infoAtomo.atomo = OP_MENOR_IGUAL;
        printf("# %d:<=\n", linha);
    }

    else if(strncmp(buffer, "==", 2) == 0) // analisa operador igual a
    {
        buffer++;buffer++;
        infoAtomo.atomo = OP_IGUAL;
        printf("# %d:==\n", linha);
    }

    else if(strncmp(buffer, "!=", 2) == 0) // analisa operador diferente de
    {
        buffer++;buffer++;
        infoAtomo.atomo = OP_DIF;
        printf("# %d:!=\n", linha);
    }

    else if(*buffer == '>') // analisa operador maior que
    {
        buffer++;
        infoAtomo.atomo = OP_MAIOR;
        printf("# %d:>\n", linha);
    }

    else if(strncmp(buffer, ">=", 2) == 0) // analisa operador maior que ou igual a
    {
        buffer++;buffer++;
        infoAtomo.atomo = OP_MAIOR_IGUAL;
        printf("# %d:>=\n", linha);
    }

    else if(*buffer == '+') // analisa operador de soma
    {
        buffer++;
        infoAtomo.atomo = OP_SOMA;
        printf("# %d:+\n", linha);
    }

    else if(*buffer == '-') // analisa operador de subtracao
    {
        buffer++;
        infoAtomo.atomo = OP_SUB;
        printf("# %d:-\n", linha);
    }

    else if(*buffer == '*') // analisa operador de multiplicacao
    {
        buffer++;
        infoAtomo.atomo = OP_MULT;
        printf("# %d:*\n", linha);
    }

    else if(*buffer == '/') // analisa operador de divisao
    {
        buffer++;
        infoAtomo.atomo = OP_DIV;
        printf("# %d:/\n", linha);
    }

    else if(*buffer == '0') // analisa numero hexadecimal
    {
        infoAtomo = reconhece_num(); // chamada de funcao de reconhecimento e conversao de numero

        if(infoAtomo.atomo == ERRO)
        {
            printf("\nerro lexico: numero invalido\n");
            exit(1);
        }

        printf("# %d:numero | %d\n", linha, infoAtomo.atributo_numero);
    }

    else if(*buffer == '\0') // analisa fim da string
    {
        infoAtomo.atomo = EOS;
    }

    infoAtomo.linha = linha;
    return infoAtomo;
}

/*
funcao de reconhecimento de identificador

reconhece o identificador, partindo do _ e armazena em infoAtomo.atributo_ID
*/

TInfoAtomo reconhece_id()
{
    TInfoAtomo infoAtomo;
    char *iniLexema;
    iniLexema = buffer; // armazena atual ponto de inicio do buffer
q0:
    if(*buffer == '_'){
        buffer++;
        goto q1;
    }
    infoAtomo.atomo = ERRO;
    return infoAtomo;
q1:
    if(isalpha(*buffer)){
        buffer++;
        goto q2;
    }
    infoAtomo.atomo = ERRO;
    return infoAtomo;

q2:
    if(isalpha(*buffer) || isdigit(*buffer))
    {
        buffer++;
        goto q2;
    }

    if( buffer-iniLexema <= 15){ // limite de 15 caracteres
        // referencia:https://cplusplus.com/reference/cstring/strncpy/
        strncpy(infoAtomo.atributo_ID,iniLexema,buffer-iniLexema);
        infoAtomo.atributo_ID[buffer-iniLexema]='\0';// finaliza string
        infoAtomo.atomo = IDENTIFICADOR;
    }
    else
        infoAtomo.atomo = ERRO;

    return infoAtomo;

}

/*
referencias: https://cplusplus.com/reference/cctype/isxdigit/
             https://cplusplus.com/reference/cstdlib/strtol/

funcao de reconhecimento de numero e conversao de hexadecimal para decimal
começa em 0x e armazena a parte hexadecimal em hexastring
armazena decimal em infoAtomo.atributo_numero
*/
TInfoAtomo reconhece_num()
{
    TInfoAtomo infoAtomo;
    char* iniLexema;
    char* hexastring;
    iniLexema = buffer; // armazena atual ponto de inicio do buffer
q0:
    if(*buffer == '0'){
        buffer++;
        goto q1;
    }

    infoAtomo.atomo = ERRO;
    return infoAtomo; // [outro]
q1:
    if(*buffer == 'x'){
        buffer++;
        goto q2;
    }
    
    infoAtomo.atomo = ERRO;
    return infoAtomo; // [outro]
q2:
    if(isxdigit(*buffer) != 0){ // verifica se o caracter é hexadecimal
        buffer++;
        goto q3;
    }
    
    infoAtomo.atomo = ERRO;
    return infoAtomo; // [outro]
q3:
    if(isxdigit(*buffer) != 0){
        buffer++;
        goto q3;
    }
    
    else if(isalpha(*buffer))
    {
        infoAtomo.atomo = ERRO;
        return infoAtomo;
    }

    strncpy(hexastring,iniLexema,buffer-iniLexema);
    hexastring[buffer-iniLexema]='\0';// finaliza string

    infoAtomo.atributo_numero = (int)strtol(hexastring,NULL,0); // converte string hexadecimal em int decimal

    infoAtomo.atomo = NUMERO;

    return infoAtomo;

}

//########### FIM Analisador Lexico ###################

//########### INICIO Analisador Sintatico ###################


//inicio da gramatica
//<programa> ::= int main “(” void “)” “{“ <declaracoes> <comandos> “}” 
void programa()
{
        consome(INTEIRO);
        
        consome(MAIN);
        
        consome(ABRE_PAR);
        
        consome(VOID_MAIN);
        
        consome(FECHA_PAR);
        
        consome(ABRE_CHAVE);
        
        declaracoes();
        
        comandos();
        
        consome(FECHA_CHAVE);

}

//<declaracoes> ::= { <declaracao> }
void declaracoes()
{
    while(lookahead == INTEIRO || lookahead == BOOLEANO)
    {
        declaracao();
    }
}

//<declaracao> ::= <tipo> <lista_variavel> “;”
void declaracao()
{
    tipo();
    lista_variavel();
    consome(PONTO_VIRGULA);
}

//<tipo> ::= int | bool
void tipo()
{
    if(lookahead == INTEIRO)
    {
        consome(INTEIRO);
    }

    else if(lookahead == BOOLEANO)
    {
        consome(BOOLEANO);
    }
}

//<lista_variavel> ::= identificador { “,” identificador }
void lista_variavel()
{
    consome(IDENTIFICADOR);

    while(lookahead == VIRGULA)
    {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
}

//<comandos> ::= { <comando> }
void comandos()
{
    while(lookahead == PONTO_VIRGULA || lookahead == ABRE_CHAVE || lookahead == IDENTIFICADOR || 
    lookahead == SE || lookahead == ENQUANTO || lookahead == ENTRADA || lookahead == SAIDA)
    {
        comando();
    }
}

/*
<comando> ::= “;”   |  
  <bloco_comandos>  |  
  <atribuicao>      |  
  <comando_if>      | 
  <comando_while>   | 
  <comando_entrada> | 
  <comando_saida> 
*/
void comando()
{
    if(lookahead == PONTO_VIRGULA)
    {
        consome(PONTO_VIRGULA);
    }

    else if(lookahead == ABRE_CHAVE)
    {
        bloco_comandos();
    }

    else if(lookahead == IDENTIFICADOR)
    {
        atribuicao();
    }

    else if(lookahead == SE)
    {
        comando_if();
    }

    else if(lookahead == ENQUANTO)
    {
        comando_while();
    }

    else if(lookahead == ENTRADA)
    {
        comando_entrada();
    }

    else if(lookahead == SAIDA)
    {
        comando_saida();
    }
}

//<bloco_comandos> ::= “{“ <comandos> “}”
void bloco_comandos()
{
    consome(ABRE_CHAVE);

    comandos();

    consome(FECHA_CHAVE);
}

//<atribuicao> ::=  identificador “=” <expressao> “;”
void atribuicao()
{
    consome(IDENTIFICADOR);

    consome(OP_ATRI);

    expressao();

    consome(PONTO_VIRGULA);

}
/*
<comando_if> ::= if “(” <expressao> “)” <comando>  
[else <comando>]
*/
void comando_if()
{
    consome(SE);

    consome(ABRE_PAR);

    expressao();

    consome(FECHA_PAR);

    comando();

    if(lookahead == SENAO)
    {
        consome(SENAO);

        comando();
    }
}

//<comando_while> ::= while “(” <expressao> “)” <comando>
void comando_while()
{
    consome(ENQUANTO);

    consome(ABRE_PAR);

    expressao();

    consome(FECHA_PAR);

    comando();
}

//<comando_entrada> ::= scanf “(“ <lista_variavel> “)” “;”
void comando_entrada()
{
    consome(ENTRADA);

    consome(ABRE_PAR);

    lista_variavel();

    consome(FECHA_PAR);

    consome(PONTO_VIRGULA);

}

//<comando_saida> ::= printf “(“ <expressao> { “,” <expressao> } “)” “;”
void comando_saida()
{
    consome(SAIDA);

    consome(ABRE_PAR);

    expressao();

    while(lookahead == VIRGULA)
    {
        consome(VIRGULA);
        expressao();
    }

    consome(FECHA_PAR);

    consome(PONTO_VIRGULA);
}

//<expressao> ::= <expressao_logica> { “||” <expressao_logica> }
void expressao()
{
    expressao_logica();

    while(lookahead == OP_OR)
    {
        consome(OP_OR);

        expressao_logica();
    }
}

//<expressao_logica>::= <expressao_relacional> { “&&” <expressao_relacional> }
void expressao_logica()
{
    expressao_relacional();

    while(lookahead == OP_AND)
    {
        consome(OP_AND);

        expressao_relacional();
    }
}
/*
<expressao_relacional> ::= <expressao_adicao>  
[ <op_relacional> <expressao_adicao> ]
*/
void expressao_relacional()
{
    expressao_adicao();

    if(lookahead == OP_MENOR || lookahead == OP_MENOR_IGUAL || lookahead == OP_IGUAL ||
    lookahead == OP_DIF || lookahead == OP_MAIOR || lookahead == OP_MAIOR_IGUAL)
    {
        op_relacional();

        expressao_adicao();
    }
}

//<op_relacional> ::= “<” | “<=” | “==” | “!=” | “>” | “>=” 
void op_relacional()
{
    if(lookahead == OP_MENOR)
    {
        consome(OP_MENOR);
    }

    else if(lookahead == OP_MENOR_IGUAL)
    {
        consome(OP_MENOR_IGUAL);
    }

    else if(lookahead == OP_IGUAL)
    {
        consome(OP_IGUAL);
    }

    else if(lookahead == OP_DIF)
    {
        consome(OP_DIF);
    }

    else if(lookahead == OP_MAIOR)
    {
        consome(OP_MAIOR);
    }

    else if(lookahead == OP_MAIOR_IGUAL)
    {
        consome(OP_MAIOR_IGUAL);
    }
}

//<expressao_adicao> ::= <expressao_multi> { (“+” | “−” ) <expressao_multi> }
void expressao_adicao()
{
    expressao_multi();

    while(lookahead == OP_SOMA || lookahead == OP_SUB)
    {
        if(lookahead == OP_SOMA)
        {
            consome(OP_SOMA);
        }
        else if(lookahead == OP_SUB)
        {
            consome(OP_SUB);
        }

        expressao_multi();
    }
}

//<expressao_multi>::= <operando> {  ( “*” | “/” ) <operando> }
void expressao_multi()
{
    operando();

    while(lookahead == OP_MULT || lookahead == OP_DIV)
    {
        if(lookahead == OP_MULT)
        {
            consome(OP_MULT);
        }
        else if(lookahead == OP_DIV)
        {
            consome(OP_DIV);
        }

        operando();
    }
}

/*
<operando> ::= identificador | 
               numero        | 
               true          | 
               false         | 
               “(“ <expressao> “)”
*/
void operando()
{
    if(lookahead == IDENTIFICADOR)
    {
        consome(IDENTIFICADOR);
    }

    else if(lookahead == NUMERO)
    {
        consome(NUMERO);
    }

    else if(lookahead == VERDADEIRO)
    {
        consome(VERDADEIRO);
    }

    else if(lookahead == FALSO)
    {
        consome(FALSO);
    }

    else if(lookahead == ABRE_PAR)
    {
        consome(ABRE_PAR);

        expressao();

        consome(FECHA_PAR);
    }
}

// compara o token obtido em lookahead com atomo, o token esperado
void consome( TAtomo atomo ){
    if (lookahead == atomo)
    {
        infoAtomo = obter_atomo();
        lookahead = infoAtomo.atomo;
    }
    else if (lookahead == ERRO)
    {
        printf("erro lexico: linha %d", infoAtomo.linha);
        exit(1);
    }
    
    
    else
    {
        printf("erro sintatico: esperado [%s] encontrado [%s]\n",strAtomo[atomo],strAtomo[lookahead]);
        exit(1);
    }

}

