OLIVER KIERAN GALVÃO MCCORMACK

--------------------------------------------------------

Este é um programa que analisa o arquivo entrada.txt e informa ao usuário se o código de entrada no arquivo texto é lexicamente e sintaticamente correto, de acordo com a linguagem fictícia "CLite".

Ele é baseado em um trabalho da faculdade de Ciência da Computação que fiz.

--------------------------------------------------------

Algumas regras léxicas:

•Caracteres Delimitadores -> Os caracteres delimitadores: espaços em branco, quebra de linhas, tabulação e retorno de carro (‘ ’, ‘\n’, ‘\t’, ‘\r’) deverão ser eliminados (ignorados) pelo analisador léxico, mas o controle de linha (contagem de linha) deverá ser mantido.

•Comentários: dois tipos de comentário, um começando com // e indo até o final da linha (1 linha) com o finalizador do comentário o caractere ‘\n’. O outro começando com /* e terminando com */ (várias linhas), nesse comentário é importante que a contagem de linha seja mantida, além disso os comentários são repassados para o analisador sintático para serem reportados e descartados.

•Palavras reservadas: As palavras reservadas na linguagem CLite são lexemas em minúsculo: bool, else, false, if, int, main, printf, scanf, true, void, while.

•Identificadores: Os identificadores começam com o caractere underline ‘_’ em seguida letra minúscula ou maiúscula, seguido de zero ou mais letras minúsculas e/ou maiúsculas, dígitos ou caractere, limitados a 15 caracteres. Caso seja encontrado um identificador com mais de 15 caracteres deve ser retornado ERRO pelo analisador léxico. A seguir a definição regular para identificadores.
letra → a|b|...|z|A|B|...|Z
digito → 0|1|...|9
identificador → _letra(letra|digito)*

Importante: Na saída do compilador, para átomo identificador, deverá ser impresso o lexema que gerou o átomo, ou seja, a sequência de caracteres reconhecida.

--------------------------------------------------------

A especificação sintática encontra-se no arquivo do código fonte.

--------------------------------------------------------

SOBRE A ENTRADA:

O programa lê o arquivo de entrada e armazena o conteúdo no buffer, não há necessidade de input do arquivo de entrada por linha de comando pelo usuário.
