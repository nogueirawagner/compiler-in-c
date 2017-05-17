#include "token_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "utils.h"

void throw_alert(int code, source_t * source) 
{
	switch (code)
	{
		case 1001:
			alert_type_int_assing_dec(source);
		default:
			0;
	}
}

/* Variavel do tipo inteira recebendo valor decimal */
int alert_type_int_assing_dec(source_t* source)
{
	fseek(source->source, source->init_pos_line, SEEK_SET);
	char buffer[255];
	FillMemory(buffer, 255, 0);

	while (1)
	{
		char value = ts_get_next_caracter(source);
		if (is_caracter_ampersand(value))
		{
			while (1)
			{
				char scopy[1] = { value };
				strncat(buffer, scopy, 1);
				value = ts_get_next_caracter(source);
				if (is_caracter_semicolon(value))
				{
					system("color E");
					printf("AL-1001 - atribuicao de valor decimal para int '%s' | linha: %i \n", buffer, source->line_cur);
					return 0;
					
				}
			}
		}
	}
	return 0;
}

int alert_unknown()
{
	printf("Alerta indefinido");
	return 0;
}
