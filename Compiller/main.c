#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include "token_stream.h"
#include "stack.h"
#include "utils.h"
#include "list.h"
#include "token_exception.h"

int main(int argc, char** argv) {

	source_t* source = ts_open_source("Source.chs"); /* Abre arquivo em bin�rio */
	stack_t* stack_token; /* Pilha de Tokens */
	int length_stack = 0;

	int ret = stack_init(&stack_token);
	if (ret < 0)
		fprintf(stderr, "Falha ao iniciar a stack \n");

	list_element_t* list_position = NULL;
	linked_list_t table_symbols;
	list_initialize(&table_symbols, NULL);

	token_t* last_tk_temp = (token_t*)malloc(sizeof(token_t));
	char* last_type = (char*)malloc(sizeof(char));

	while (1)
	{
		token_t * token = ts_get_next_token(source, last_tk_temp, last_type);  /* Pega proximo token */

		/* Insere token na pilha */
		if (token != NULL && is_token_valid(token, source))
		{
			if (token->type == TK_TYPE)
				last_type = token->id;

			stack_push(&stack_token, token);
			length_stack++;
			last_tk_temp = token;
		}

		if (is_caracter_semicolon(source->last_read))
		{
			stack_t* ids; /* variaveis */
			stack_t* constants; /* atribuicoes de valores */
			stack_init(&ids);
			stack_init(&constants);

			token_t* last_tk = (token_t*)stack_pop(&stack_token);
			int count_id = 0;
			int count_const = 0;
			int count_semicolon = 0;

			if (last_tk && last_tk->type == TK_STM_END) 
			{
				count_semicolon++;
				last_tk = (token_t*)stack_pop(&stack_token);
			}

			while (last_tk && last_tk->type == TK_CONST)
			{
				stack_push(&constants, last_tk);
				last_tk = (token_t*)stack_pop(&stack_token);
				count_const++;

				if (last_tk && last_tk->type == TK_EQUAL)
				{
					last_tk = (token_t*)stack_pop(&stack_token);
					if (last_tk && last_tk->type != TK_ID)
						throw_exception(1002, source->line_cur, source);

					stack_push(&ids, last_tk);
					count_id++;
					last_tk = (token_t*)stack_pop(&stack_token);
				}
				else
					throw_exception(1002, source->line_cur, source);
			}
#pragma region Declaracao de variavel simples Ex: int &var, &var1;

			while (last_tk && last_tk->type == TK_ID)
			{
				stack_push(&ids, last_tk);
				last_tk = (token_t*)stack_pop(&stack_token);
				count_id++;

				while (last_tk->type == TK_CONST)
				{
					stack_push(&constants, last_tk);
					last_tk = (token_t*)stack_pop(&stack_token);
					count_const++;

					if (last_tk && last_tk->type == TK_EQUAL)
					{
						last_tk = (token_t*)stack_pop(&stack_token);
						if (last_tk && last_tk->type != TK_ID)
							throw_exception(1002, source->line_cur, source);
						stack_push(&ids, last_tk);
						count_id++;
						last_tk = (token_t*)stack_pop(&stack_token);
					}
					else
						throw_exception(1002, source->line_cur, source);
				}
			}
#pragma endregion

			if (last_tk && last_tk->type == TK_TYPE)
			{
				while (count_id > 0)
				{
					token_t* id = stack_pop(&ids);
					token_t* valor = stack_pop(&constants);
					count_id--;
					count_const--;
					char* length = 0; // tamanho de variavel char e dec 

					char* _dec = "dec";
					char* _char = "char";

					if (id && id->type == TK_ID && ts_are_equal(last_tk->id, _char))
						 length = any_definition_length(id->id, source, 0);
					if (id && id->type == TK_ID && ts_are_equal(last_tk->id, _dec))
						length = any_definition_length(id->id, source, 1);


					table_symbols_t* tbs = (table_symbols_t*)malloc(sizeof(table_symbols_t));
					tbs->type = last_tk->id;
					tbs->line = last_tk->line;

					if (!length)
						tbs->length = "NULL";
					else
						tbs->length = length;

					if (!valor)
						tbs->value = "NULL";
					else
						tbs->value = valor->id;
					tbs->variable = id->id;

					/* Verificar se item existe na tabela de s�mbolos */
					if (table_symbols.size == 0)
					{
						if (list_any_tbl_symb(&table_symbols, list_position, tbs->variable, tbs->type))
							throw_exception(1004, source->line_cur, source);
						else
						{
							list_insert_next(&table_symbols, NULL, tbs);
							list_position = list_head(&table_symbols);
						}
					}
					else
					{
						if (list_any_tbl_symb(&table_symbols, list_position, tbs->variable, tbs->type))
							throw_exception(1004, source->line_cur, source);
						else
						{
							list_insert_next(&table_symbols, list_position, tbs);
							list_position = list_head(&table_symbols);
						}
					}
				}
			}

			if (count_id > 0 || count_const > 0)
			{
				while (count_id != 0)
				{
					token_t* id = stack_pop(&ids);
					token_t* valor = stack_pop(&constants);
					count_id--;
					count_const--;
					int i = 0;

					if (table_symbols.size == 0)
						throw_exception(1003, source->line_cur, source);
					if (!list_any_tbl_symb(&table_symbols, list_position, id->id, NULL))
						throw_exception(1003, source->line_cur, source);
					else
						list_update_tbl_symb(&table_symbols, list_position, id->id, valor->id);
				}
			}
		}

#pragma region Exibir tabela de simbolos
		if (source->last_read == -1)
		{
			printf("Tabela de simbolos \n");
			printf("\n");
			printf("\t%-3s\t| %-15s\t| %-2s\t| %-20s\t|\t %-10s\n", "TIPO", "VARIAVEL", "TAM", "VALOR", "LINHA");
			printf("\t------------------------------------------------------------------------------------");
			printf("\n");
			for (int i = 0; i < list_get_size(&table_symbols); i++)
			{
				table_symbols_t* object = (table_symbols_t*)list_position->data;

				char* variable = (char*)object->variable;
				char* tipo = (char*)object->type;
				char* value = (char*)object->value;
				char* length = (char*)object->length;
				int line = object->line;

				printf("\t%-3s\t| %-15s\t| %-2s\t| %-20s\t|\t %-10i\n", tipo, variable, length, value, line);
				list_position = list_next(list_position);
			}
		}
#pragma endregion
	}

error:
	"erro ao processar";
	ts_close_source(source);
	return 0;

	getchar();
}