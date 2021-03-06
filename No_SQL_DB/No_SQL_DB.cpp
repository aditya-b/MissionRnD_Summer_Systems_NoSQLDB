// No_SQL_DB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<stdlib.h>
#include<string.h>

/*struct json
{
	char **params;
	char **values;
	int cols;
	int version;
};

struct row
{
	int commit_version;
	int current_version;
	int id;
	struct json **records;
	struct json *display_record;
};

struct table
{
	int id;
	char *name;
	struct row **rows;
};

struct hash
{
	int key;
	int value;
}**hash_table;

int get_column(char **params, char *param, int cols)
{
	int i;
	for (i = 0; i < cols; i++)
		if (!strcmp(params[i], param))
			return i;
	return -1;
}

int get_row(table *tables, int rows, int row_id)
{
	int i;
	for (i = 0; i < rows; i++)
		if (tables->rows[i]->id == row_id)
			return i;
	return -1;
}

int put(table **table, int row_id, int version, json data,int *rows)
{
	int i;
	int position;
	if (*rows==0)
		position = -1;
	else
		position=get_row(*table, *rows, row_id);
	if (position == -1)
	{
		(*table)->rows = (row**)realloc((*table)->rows, sizeof(row)*(*rows + 1));
		(*table)->rows[*rows] = (row*)malloc(sizeof(row));
		(*table)->rows[*rows]->commit_version = 1;
		(*table)->rows[*rows]->current_version = 1;
		(*table)->rows[*rows]->id = row_id;
		(*table)->rows[*rows]->records = (json**)malloc(sizeof(json));
		(*table)->rows[*rows]->records[0] = (json*)malloc(sizeof(json));
		(*table)->rows[*rows]->records[0]->cols = data.cols;
		(*table)->rows[*rows]->records[0]->params = (char**)malloc(sizeof(char*)*data.cols);
		for (i = 0; i < data.cols; i++)
		{
			(*table)->rows[*rows]->records[0]->params[i] = (char*)malloc(sizeof(char*));
			strcpy((*table)->rows[*rows]->records[0]->params[i], data.params[i]);
		}
		(*table)->rows[*rows]->records[0]->values = (char**)malloc(sizeof(char*)*data.cols);
		for (i = 0; i < data.cols; i++)
		{
			(*table)->rows[*rows]->records[0]->values[i] = (char*)malloc(sizeof(char*));
			strcpy((*table)->rows[*rows]->records[0]->values[i], data.values[i]);
		}
		(*table)->rows[*rows]->records[0]->version = 1;
		(*table)->rows[*rows]->display_record = (*table)->rows[*rows]->records[0];
		*rows += 1;
		return 1;
	}
	else if (version == (*table)->rows[position]->commit_version)
	{
		int x = (*table)->rows[position]->current_version;
		(*table)->rows[position]->records = (json**)realloc((*table)->rows[position]->records, sizeof(json)*(x + 1));
		(*table)->rows[position]->records[x] = (json*)malloc(sizeof(json));
		(*table)->rows[position]->records[x]->cols = data.cols;
		(*table)->rows[position]->records[x]->params = (char**)malloc(sizeof(char*)*data.cols);
		for (i = 0; i < data.cols; i++)
		{
			(*table)->rows[position]->records[x]->params[i] = (char*)malloc(sizeof(char*));
			strcpy((*table)->rows[position]->records[x]->params[i], data.params[i]);
		}
		(*table)->rows[position]->records[x]->values = (char**)malloc(sizeof(char*)*data.cols);
		for (i = 0; i < data.cols; i++)
		{
			(*table)->rows[position]->records[x]->values[i] = (char*)malloc(sizeof(char*));
			strcpy((*table)->rows[position]->records[x]->values[i], data.values[i]);
		}
		(*table)->rows[position]->records[x]->version = x + 1;
		json *cell = (*table)->rows[position]->display_record;
		(*table)->rows[position]->display_record = (json*)malloc(sizeof(json));
		(*table)->rows[position]->display_record->params = (char**)malloc(sizeof(char*)*cell->cols);
		(*table)->rows[position]->display_record->values = (char**)malloc(sizeof(char*)*cell->cols);
		(*table)->rows[position]->display_record->cols = cell->cols;
		int cols = cell->cols;
		for (i = 0; i < cols; i++)
		{
			(*table)->rows[position]->display_record->params[i] = (char*)malloc(sizeof(char*));
			strcpy((*table)->rows[position]->display_record->params[i], cell->params[i]);
			(*table)->rows[position]->display_record->values[i] = (char*)malloc(sizeof(char*));
			strcpy((*table)->rows[position]->display_record->values[i], cell->values[i]);
		}
		for (i = 0; i < data.cols; i++)
		{
			int p = get_column(cell->params, data.params[i], cell->cols);
			if (p != -1)
			{
				(*table)->rows[position]->display_record->params[p] = (char*)malloc(sizeof(char*));
				strcpy((*table)->rows[position]->display_record->params[p], data.params[i]);
				(*table)->rows[position]->display_record->values[p] = (char*)malloc(sizeof(char*));
				strcpy((*table)->rows[position]->display_record->values[p], data.values[i]);
			}
			else
			{
				(*table)->rows[position]->display_record->params = (char**)realloc((*table)->rows[position]->display_record->params, sizeof(char*)*(cols + 1));
				(*table)->rows[position]->display_record->params[cols] = (char*)malloc(sizeof(char*));
				strcpy((*table)->rows[position]->display_record->params[cols], data.params[i]);
				(*table)->rows[position]->display_record->values = (char**)realloc((*table)->rows[position]->display_record->values, sizeof(char*)*(cols + 1));
				(*table)->rows[position]->display_record->values[cols] = (char*)malloc(sizeof(char*));
				strcpy((*table)->rows[position]->display_record->values[cols], data.values[i]);
				cols++;
			}
		}
		(*table)->rows[position]->display_record->cols = cols;
		(*table)->rows[position]->display_record->version = x + 1;
		(*table)->rows[position]->commit_version = x + 1;
		(*table)->rows[position]->current_version = x + 1;
		return x + 1;
	}
	else
		return -1;
}

json* get_data_json(char *filename,int *id,int start)
{
	int row_id=0;
	int cols = 0;
	int x = start;
	char dat[40];
	int y = 0;
	json *data=(json*)malloc(sizeof(json));
	data->params = (char**)malloc(sizeof(char*));
	data->values = (char**)malloc(sizeof(char*));
	data->version = 1;
	while (filename[x] != ' '&&filename[x] != '\0'&&filename[x] != '\n')
		row_id = row_id * 10 + (filename[x++] - 48);
	x++;
	while (1)
	{
		y = 0;
		while (filename[x] != ' '&&filename[x] != '\0'&&filename[x] != '\n')
			dat[y++] = filename[x++];
		dat[y] = 0;
		x++;
		data->params = (char**)realloc(data->params, sizeof(char*)*(cols + 1));
		data->values = (char**)realloc(data->values, sizeof(char*)*(cols + 1));
		data->params[cols] = (char*)malloc(sizeof(char*));
		data->values[cols] = (char*)malloc(sizeof(char*));
		int k = 0;
		y = 0;
		while (dat[y] != ':')
			data->params[cols][k++] = dat[y++];
		data->params[cols][k] = '\0';
		y++;
		k = 0;
		while (dat[y] != '\0')
			data->values[cols][k++] = dat[y++];
		data->values[cols][k] = '\0';
		if (filename[x - 1] == '\0' || filename[x - 1] == '\n')
			break;
		cols++;
	}
	data->cols = cols + 1;
	*id = row_id;
	return data;
}

void flush_to_file(table **tables, int table_no, int *rows, int **versions)
{
	char filename[50];
	FILE *fp;
	int i, j, k, l;
	for (i = 0; i < table_no; i++)
	{
		for (j = 0; j < rows[i]; j++)
		{
			for (k = versions[i][j]-1; k >=0 ; k--)
			{
				for (l = 0; l < tables[i]->rows[j]->records[k]->cols; l++)
				{
					sprintf(filename, "DataFiles/%s_%s.txt", tables[i]->name, tables[i]->rows[j]->records[k]->params[l]);
					fp = fopen(filename, "a");
					fprintf(fp, "%d %s %d\n", tables[i]->rows[j]->id , tables[i]->rows[j]->records[k]->values[l], tables[i]->rows[j]->records[k]->version);
					fclose(fp);
				}
			}
		}
	}
	for (i = 0; i < table_no; i++)
	{
		sprintf(filename, "%s_meta_data.txt", tables[i]->name);
		fopen(filename, "a");
		for (j = 0; j < rows[i]; j++)
			fprintf(fp, "%d %d\n", tables[i]->rows[j]->commit_version);
	}
}

int get_table(table **tables, char *table_name, int table_length)
{
	int i;
	for (i = 0; i < table_length; i++)
		if (!_strcmpi(tables[i]->name, table_name))
			return i;
	return -1;
}

int main()
{
	char query[100];
	int table_no = 0;
	table **tables = (table**)malloc(sizeof(table));
	int *rows = (int*)malloc(sizeof(int));
	int **versions = (int**)malloc(sizeof(int));
	while (1)
	{
		printf("\nDBQUERY> ");
		gets_s(query);
		char op[10];
		char table_name[20];
		int i = 0;
		while (query[i] != ' '&&query[i]!='\0')
			op[i] = query[i++];
		op[i] = '\0';
		if(query[i])
			i++;
		int x = 0;
		while (query[i] != '\0'&&query[i]!=' ')
			table_name[x++] = query[i++];
		table_name[x] = '\0';
		if(query[i])
			i++;
		if (!_stricmp(op, "CREATE"))
		{
			tables = (table**)realloc(tables, sizeof(table)*(table_no + 1));
			versions = (int**)realloc(versions, sizeof(int*)*(table_no + 1));
			versions[table_no] = (int*)malloc(sizeof(int));
			rows = (int*)realloc(rows, sizeof(int)*(table_no + 1));
			rows[table_no] = 0;
			tables[table_no] = (table*)malloc(sizeof(table));
			tables[table_no]->rows = (row**)malloc(sizeof(row));
			tables[table_no]->name = (char*)malloc(sizeof(char*));
			strcpy(tables[table_no]->name, table_name);
			table_no++;
			printf("Table '%s' created successfully!\n", table_name);
		}
		else if (!_stricmp(op, "PUT"))
		{
			int pos = get_table(tables, table_name, table_no);
			if (pos == -1)
				printf("Error: Table '%s' not found!", table_name);
			else
			{
				versions[pos] = (int*)realloc(versions[pos], sizeof(int)*(rows[pos] + 1));
				int row_id = 0;
				json *data = get_data_json(query, &row_id, i);
				int p = get_row(tables[pos], rows[pos], row_id);
				if (p == -1)
					p = rows[pos];
				int a = put(&tables[pos], row_id, versions[pos][p], *data, &rows[pos]);
				if (a != -1)
				{
					versions[pos][p] = a;
					printf("PUT for row_id '%d' successful!\n", row_id);
				}
				else
					printf("Error: Commit conflict!\n");
			}
		}
		else if (!_stricmp(op, "GET"))
		{
			int pos = get_table(tables, table_name, table_no);
			if (pos == -1)
				printf("Error: Table '%s' not found!", table_name);
			else
			{
				int row_id = 0;
				while (query[i] != '\0')
					row_id = row_id * 10 + (query[i++] - 48);
				int p = get_row(tables[pos], rows[pos], row_id);
				if (p == -1)
					printf("Error: Row_ID not found!\n");
				else
				{
					printf("Data JSON:\n{\n\t'row_id':%d\n", row_id);
					int i = 0;
					for (i = 0; i < tables[pos]->rows[p]->display_record->cols; i++)
						printf("\t'%s':'%s'\n", tables[pos]->rows[p]->display_record->params[i], tables[pos]->rows[p]->display_record->values[i]);
					printf("\t'version':'%d'\n}\n", tables[pos]->rows[p]->display_record->version);
					versions[pos][p] = tables[pos]->rows[p]->display_record->version;
				}
			}
		}
		else if (!_stricmp(op, "DELETE"))
		{
			int pos = get_table(tables, table_name, table_no);
			if (pos == -1)
				printf("Error: Table '%s' not found!", table_name);
			else
			{
				int row_id = 0;
				while (query[i] != '\0')
					row_id = row_id * 10 + (query[i++] - 48);
				int p = get_row(tables[pos], rows[pos], row_id);
				if (p == -1)
					printf("Error: Row_ID not found!\n");
				else
				{
					tables[pos]->rows[p] = tables[pos]->rows[rows[pos] - 1];
					versions[pos][p] = versions[pos][rows[pos] - 1];
					rows[pos] -= 1;
					tables[pos]->rows = (row**)realloc(tables[pos]->rows, sizeof(table)*rows[pos]);
					versions[pos] = (int*)realloc(versions[pos], sizeof(int)*rows[pos]);
					printf("Record successfully deleted!\n");
				}
			}
		}
		else if(!_strcmpi("FLUSH",op))
			flush_to_file(tables,table_no,rows,versions);
		else if (!_strcmpi("EXIT", op))
			exit(0);
		else
			printf("Error: Invalid Command!\n");
	}
	return 0;
}*/
