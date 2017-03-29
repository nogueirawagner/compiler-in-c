#include "token_stream.h"
#include <Windows.h>

/* Verifica se � caracter A...Z ou a...z */
int is_alphanumeric(char value)
{
	return (value >= 65 && value <= 90) || (value >= 97 && value <= 122);
}

/* Verifica se � n�merico */
int is_numeric(char value)
{
	return (value >= 48 && value <= 57);
}

/* Verifica se � caracter e-comercial '&' */
int is_caracter_ampersand(char value)
{
	return (value == 38);
}

/* Verifica se � caracter ponto e v�rgula ';' */
int is_caracter_semicolon(char value)
{
	return (value == 59);
}

/* Verifica se � caracter v�rgula ',' */
int is_caracter_comma(char value)
{
	return (value == 44);
}

/* Verifica se � operador de igualdade '=' */
int is_caracter_equals(char value)
{
	return (value == 61);
}

/* Verifica se � espa�o em branco */
int is_space(char value)
{
	return (value == 32);
}

/* Verifica se � final de linha */
int is_new_line(char value)
{
	return (value == 13 || value == 10);
}

/* Verifica se � um caracter '\n' */
int is_caracter_smash_line(char value)
{
	return (value == 10);
}

/* Verifica se � abre chaves '{' */
int is_caracter_key_opened(char value)
{
	return (value == 123);
}

/* Verifica se � caracter '}' */
int is_caracter_key_closed(char value)
{
	return (value == 125);
}

/* Verifica se caracter � letra 'm' */
int is_caracter_m(char value) 
{
	return (value == 109);
}

/* Verificar se ponteiros s�o iguais */
int ts_are_equal(char* pointer1, char* pointer2)
{
	return !(strcmp(pointer1, pointer2));
}

/* Define se o token � um tipo de dado */
int is_token_type_data(char* value)
{
	char * tipos[4] = { "int", "float", "dec", "char" };

	for (int i = 0; i < 3; i++)
	{
		if (strcmp(tipos[i], value) == 0)
			return 1;
	}
	return 0;
}

/* Define se o token � uma vari�vel */
int is_token_variable(char* value)
{
	return is_caracter_ampersand(*value);
}

/* Retorna o tamanho do conte�do do ponteiro */
int length_content_token(char* value) 
{
	int b = 0;
	int length = 0;
	while (value[b] != 0)
	{
		if (value[b] != 0)
			length++;
		else
			return length;
		b++;
	}
	return length;
}

/* Retorna o valor do ponteiro */
char* value_content(char* value) 
{
	char* buffer = (char*)malloc(255);
	FillMemory(buffer, 255, 0);
	int tam = length_content_token(value);

	for (int j = 0; j < tam; j++)
	{
		char scopy[1] = { value[j] };
		strncat(buffer, scopy, 1);
	}
	return buffer;
}