// No_SQL_DB.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<stdlib.h>
#include<string.h>
#include<conio.h>

struct json
{
	char **params;
	char **values;
	int cols;
	int version;
	json *next;
};

struct row
{
	int commit_version;
	int current_version;
	int id;
	json *records;
	json *display_record;
	row *next;
};

struct table
{
	int id;
	char *name;
	struct row *rows;
	struct row *last;
};

struct hash
{
	int key;
	int value;
}**hash_table;

json* version_exists(row *start, int version)
{
	json *head = start->records;
	while (head)
	{
		if (head->version == version)
			return head;
		head = head->next;
	}
	return NULL;
}
int get_column(char **params, char *param, int cols)
{
	int i;
	for (i = 0; i < cols; i++)
		if (!strcmp(params[i], param))
			return i;
	return -1;
}

row* get_row(table *tables, int row_id)
{
	row *start = tables->rows;
	while (start)
	{
		if (start->id == row_id)
			return start;
		start = start->next;
	}
	return NULL;
}

int get_row_index(table *tables, int row_id)
{
	row *start = tables->rows;
	int q = 0;
	while (start)
	{
		if (start->id == row_id)
			return q;
		start = start->next;
		q++;
	}
	return -1;
}

int put(table **table, int row_id, int version, json data)
{
	int i;
	row* position = get_row(*table, row_id);
	row *dummy;
	if (position == NULL)
	{
		if ((*table)->rows == NULL)
		{
			(*table)->rows = (row*)malloc(sizeof(row));
			dummy = (*table)->rows;
		}
		else
		{
			dummy = (*table)->rows->next;
			row *link = (*table)->rows;
			while (dummy)
			{
				link = dummy;
				dummy = dummy->next;
			}
			dummy = (row*)malloc(sizeof(row));
			link->next = dummy;
		}
		dummy->commit_version = version;
		dummy->current_version = version;
		dummy->id = row_id;
		dummy->next = NULL;
		dummy->records = (json*)malloc(sizeof(json));
		dummy->display_record = dummy->records;
		dummy->records->cols = data.cols;
		dummy->records->next = NULL;
		dummy->records->version = 1;
		dummy->records->params = (char**)malloc(sizeof(char*)*(data.cols));
		dummy->records->values = (char**)malloc(sizeof(char*)*(data.cols));
		for (i = 0; i < data.cols; i++)
		{
			dummy->records->params[i] = (char*)malloc(sizeof(char)*strlen(data.params[i]));
			dummy->records->values[i] = (char*)malloc(sizeof(char)*strlen(data.values[i]));
			strcpy(dummy->records->params[i], data.params[i]);
			strcpy(dummy->records->values[i], data.values[i]);
		}
		return 1;
	}
	else if(version == position->commit_version)
	{
		position->commit_version = position->commit_version + 1;
		position->current_version = position->current_version + 1;
		json *cell = position->display_record;
		position->display_record = (json*)malloc(sizeof(json));
		position->display_record->cols = cell->cols;
		position->display_record->next = NULL;
		position->display_record->version = cell->version + 1;
		position->display_record->params = (char**)malloc(sizeof(char*)*cell->cols);
		position->display_record->values = (char**)malloc(sizeof(char*)*cell->cols);
		for (i = 0; i < cell->cols; i++)
		{
			position->display_record->params[i] = (char*)malloc(sizeof(char)*strlen(cell->params[i]));
			position->display_record->values[i] = (char*)malloc(sizeof(char)*strlen(cell->values[i]));
			strcpy(position->display_record->params[i], cell->params[i]);
			strcpy(position->display_record->values[i], cell->values[i]);
		}
		int cols = cell->cols;
		json *d = position->records;
		position->records = (json*)malloc(sizeof(json));
		position->records->cols = data.cols;
		position->records->next = d;
		position->records->version = cell->version + 1;
		position->records->params = (char**)malloc(sizeof(char*)*data.cols);
		position->records->values = (char**)malloc(sizeof(char*)*data.cols);
		for (i = 0; i < data.cols; i++)
		{
			position->records->params[i] = (char*)malloc(sizeof(char)*30);
			position->records->values[i] = (char*)malloc(sizeof(char)*30);
			strcpy(position->records->params[i], data.params[i]);
			strcpy(position->records->values[i], data.values[i]);
			int p = get_column(position->display_record->params, data.params[i], position->display_record->cols);
			if (p == -1)
			{
				position->display_record->params = (char**)realloc(position->display_record->params, sizeof(char*)*(cols + 1));
				position->display_record->values = (char**)realloc(position->display_record->values, sizeof(char*)*(cols + 1));
				position->display_record->params[cols] = (char*)malloc(sizeof(char)*strlen(data.params[i]));
				position->display_record->values[cols] = (char*)malloc(sizeof(char)*strlen(data.values[i]));
				strcpy(position->display_record->params[cols], data.params[i]);
				strcpy(position->display_record->values[cols], data.values[i]);
				cols++;
			}
			else
			{
				position->display_record->params[p] = (char*)realloc(position->display_record->params[p], sizeof(char)* strlen(data.params[i]));
				position->display_record->values[p] = (char*)realloc(position->display_record->values[p], sizeof(char)* strlen(data.values[i]));
				strcpy(position->display_record->params[p], data.params[i]);
				strcpy(position->display_record->values[p], data.values[i]);
			}
		}
		position->display_record->cols = cols;
		return position->commit_version;
	}
	return -1;
}

json* get_data_json(char *filename, int *id, int start)
{
	int row_id = 0;
	int cols = 0;
	int x = start;
	char dat[60];
	int y = 0;
	json *data = (json*)malloc(sizeof(json));
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
		dat[y] = '\0';
		x++;
		(*data).params = (char**)realloc((*data).params, sizeof(char*)*(cols + 1));
		(*data).values = (char**)realloc((*data).values, sizeof(char*)*(cols + 1));
		data->params[cols] = (char*)malloc(sizeof(char)*30);
		data->values[cols] = (char*)malloc(sizeof(char)*30);
		int k = 0;
		y = 0;
		char d[30];
		while (dat[y] != ':')
			d[k++] = dat[y++];
		d[k] = '\0';
		strcpy(data->params[cols], d);
		y++;
		k = 0;
		while (dat[y] != '\0')
			d[k++] = dat[y++];
		d[k] = '\0';
		strcpy(data->values[cols], d);
		if (filename[x - 1] == '\0' || filename[x - 1] == '\n')
			break;
		cols++;
	}
	data->cols = cols + 1;
	*id = row_id;
	return data;
}

void flush_to_file(table **tables, int table_no)
{
	char filename[50];
	FILE *fp;
	int i, j, k, l;
	for (i = 0; i < table_no; i++)
	{
		row *row_head = tables[i]->rows;
		while (row_head)
		{
			json *data_head = row_head->records;
			while (data_head)
			{
				for (j = 0; j < data_head->cols; j++)
				{
					sprintf(filename, "DataFiles/%s_%s.txt", tables[i]->name, data_head->params[j]);
					fp=fopen(filename, "a");
					fprintf(fp, "%d %s %d\n", row_head->id, data_head->values[j], data_head->version);
					fclose(fp);
				}
				data_head = data_head->next;
			}
			row_head = row_head->next;
		}

	}
	for (i = 0; i < table_no; i++)
	{
		sprintf(filename, "%s_meta_data.txt", tables[i]->name);
		fp=fopen(filename, "a");
		row *row_head = tables[i]->rows;
		while (row_head)
		{
			fprintf(fp, "%d %d\n", row_head->id, row_head->commit_version);
			row_head = row_head->next;
		}
		fclose(fp);
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

void load_from_files(table **tables, char *column_name, char *filename, int *rows)
{
	FILE *fp = fopen(filename, "r");
	char row_data[60];
	if (fp == NULL)
		printf("Error: File not found!\n\n");
	else
	{
		while (fgets(row_data, 60, fp))
		{
			int row_id = 0;
			char data[40];
			int version = 0;
			int i = 0, x = 0;
			while (row_data[i] != ' ')
				row_id = row_id * 10 + (row_data[i++] - 48);
			i++;
			while (row_data[i] != ' ')
				data[x++] = row_data[i++];
			data[x] = '\0';
			i++;
			while (row_data[i] != '\0' && row_data[i] != '\n')
				version = version * 10 + (row_data[i++] - 48);
			row *id = get_row(*tables, row_id);
			if (id == NULL)
			{
				row *dummy;
				if ((*tables)->rows == NULL)
				{
					(*tables)->rows = (row*)malloc(sizeof(row));
					dummy = (*tables)->rows;
				}
				else
				{
					dummy = (*tables)->rows->next;
					row *link = (*tables)->rows;
					while (dummy)
					{
						link = dummy;
						dummy = dummy->next;
					}
					dummy= (row*)malloc(sizeof(row));
					link->next = dummy;
				}
				dummy->next = NULL;
				dummy->commit_version = version;
				dummy->current_version = version;
				dummy->id = row_id;
				dummy->records = (json*)malloc(sizeof(json));
				dummy->display_record = (json*)malloc(sizeof(json));
				dummy->display_record->cols = 1;
				dummy->display_record->next = NULL;
				dummy->display_record->version = version;
				dummy->display_record->params = (char**)malloc(sizeof(char*));
				dummy->display_record->values = (char**)malloc(sizeof(char*));
				dummy->display_record->params[0] = (char*)malloc(sizeof(char)*strlen(column_name));
				dummy->display_record->values[0] = (char*)malloc(sizeof(char)*strlen(data));
				strcpy(dummy->display_record->params[0], column_name);
				strcpy(dummy->display_record->values[0], data);
				dummy->records->cols = 1;
				dummy->records->next = NULL;
				dummy->records->version = version;
				dummy->records->params = (char**)malloc(sizeof(char*));
				dummy->records->values = (char**)malloc(sizeof(char*));
				dummy->records->params[0] = (char*)malloc(sizeof(char)*strlen(column_name));
				dummy->records->values[0] = (char*)malloc(sizeof(char)*strlen(data));
				strcpy(dummy->records->params[0], column_name);
				strcpy(dummy->records->values[0], data);
			}
			else
			{
				json *json_data = version_exists(id, version);
				if (json_data == NULL)
				{
					json *offset = id->records;
					if (id->commit_version < version)
					{
						id->records=  (json*)malloc(sizeof(json));
						id->records->cols = 1;
						id->records->next = offset;
						id->records->version = version;
						id->records->params = (char**)malloc(sizeof(char*));
						id->records->values = (char**)malloc(sizeof(char*));
						id->records->params[0] = (char*)malloc(sizeof(char)*strlen(column_name));
						id->records->values[0] = (char*)malloc(sizeof(char)*strlen(data));
						strcpy(id->records->params[0], column_name);
						strcpy(id->records->values[0], data);
						id->commit_version = version;
						id->current_version = version;
					}
					else
					{
						json *orig;
						while (offset->next != NULL && offset->version > version)
						{
							offset = offset->next;
							orig = offset;
						}
						if (offset->next == NULL)
						{
							offset->next = (json*)malloc(sizeof(json));
							orig = offset->next;
							orig->next = NULL;
						}
						else
						{
							orig->next = (json*)malloc(sizeof(json));
							orig->next->next = offset;
							orig = orig->next;
						}
						orig->cols = 1;
						orig->version = version;
						if (id->commit_version < version)
							id->commit_version = version;
						if (id->current_version < version)
							id->current_version = version;
						orig->params = (char**)malloc(sizeof(char*));
						orig->values = (char**)malloc(sizeof(char*));
						orig->params[0] = (char*)malloc(sizeof(char)*strlen(column_name));
						orig->values[0] = (char*)malloc(sizeof(char)*strlen(data));
						strcpy(orig->params[0], column_name);
						strcpy(orig->values[0], data);
					}
					int i = get_column(id->display_record->params, column_name, id->display_record->cols);
					if (i == -1)
					{
						id->display_record->cols += 1;
						int col = id->display_record->cols - 1;
						id->display_record->params = (char**)realloc(id->display_record->params, sizeof(char*)*id->display_record->cols);
						id->display_record->values = (char**)realloc(id->display_record->values, sizeof(char*)*id->display_record->cols);
						id->display_record->params[col] = (char*)malloc(sizeof(char)*strlen(column_name));
						id->display_record->values[col] = (char*)malloc(sizeof(char)*strlen(data));
						id->display_record->next = NULL;
						strcpy(id->display_record->params[col], column_name);
						strcpy(id->display_record->values[col], data);
						if (version > id->display_record->version)
							id->display_record->version = version;
					}
					else if(id->commit_version<=version)
					{
						json_data->params[i] = (char*)realloc(json_data->params[i], sizeof(char)*strlen(column_name));
						json_data->values[i] = (char*)realloc(json_data->values[i], sizeof(char)*strlen(data));
						strcpy(json_data->params[i], column_name);
						strcpy(json_data->values[i], data);
					}
				}
				else
				{
					int i = get_column(json_data->params, column_name, json_data->cols);
					if (i == -1)
					{
						json_data->cols += 1;
						json_data->params = (char**)realloc(json_data->params, sizeof(char*)*json_data->cols);
						json_data->values = (char**)realloc(json_data->values, sizeof(char*)*json_data->cols);
						int col = json_data->cols - 1;
						json_data->params[col] = (char*)malloc(sizeof(char)*strlen(column_name));
						json_data->values[col] = (char*)malloc(sizeof(char)*strlen(data));
						strcpy(json_data->params[col], column_name);
						strcpy(json_data->values[col], data);
						id->display_record->cols += 1;
						col = id->display_record->cols - 1;
						id->display_record->params = (char**)realloc(id->display_record->params, sizeof(char*)*id->display_record->cols);
						id->display_record->values = (char**)realloc(id->display_record->values, sizeof(char*)*id->display_record->cols);
						id->display_record->params[col] = (char*)malloc(sizeof(char)*strlen(column_name));
						id->display_record->values[col] = (char*)malloc(sizeof(char)*strlen(data));
						strcpy(id->display_record->params[col], column_name);
						strcpy(id->display_record->values[col], data);
					}
					else
					{
						json_data->params[i] = (char*)realloc(json_data->params[i],sizeof(char)*strlen(column_name));
						json_data->values[i] = (char*)realloc(json_data->values[i],sizeof(char)*strlen(data));
						strcpy(json_data->params[i], column_name);
						strcpy(json_data->values[i], data);
					}
				}
			}
		}
	}
	fclose(fp);
}

int main()
{
	char query[150];
	int table_no = 0;
	table **tables = (table**)malloc(sizeof(table));
	int *rows = (int*)malloc(sizeof(int));
	int **versions = (int**)malloc(sizeof(int));
	while (1)
	{
		printf("\nDBQUERY> ");
		_getch;
		gets_s(query);
		char op[10];
		char table_name[20];
		int i = 0;
		while (query[i] != ' '&&query[i] != '\0')
			op[i] = query[i++];
		op[i] = '\0';
		if (query[i])
			i++;
		int x = 0;
		while (query[i] != '\0'&&query[i] != ' ')
			table_name[x++] = query[i++];
		table_name[x] = '\0';
		if (query[i])
			i++;
		if (!_stricmp(op, "CREATE"))
		{
			tables = (table**)realloc(tables, sizeof(table)*(table_no + 1));
			versions = (int**)realloc(versions, sizeof(int*)*(table_no + 1));
			versions[table_no] = (int*)malloc(sizeof(int));
			rows = (int*)realloc(rows, sizeof(int)*(table_no + 1));
			rows[table_no] = 0;
			tables[table_no] = (table*)malloc(sizeof(table));
			tables[table_no]->rows = NULL;
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
				versions[pos][rows[pos]] = 0;
				int row_id = 0;
				json *data = get_data_json(query, &row_id, i);
				int p = get_row_index(tables[pos], row_id);
				if (p == -1)
					p = rows[pos];
				int a = put(&tables[pos], row_id, versions[pos][p]+1, *data);
				if (a != -1)
				{
					versions[pos][p] = a;
					if (a == 1)
						rows[pos] += 1;
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
				row* p = get_row(tables[pos], row_id);
				int x = get_row_index(tables[pos], row_id);
				if (p == NULL)
					printf("Error: Row_ID not found!\n");
				else
				{
					printf("Data for row_id- %d:\n{\n\t'row_id':%d\n", row_id,row_id);
					int i = 0;
					for (i = 0; i < p->display_record->cols; i++)
						printf("\t'%s':'%s'\n", p->display_record->params[i], p->display_record->values[i]);
					printf("\t'version':'%d'\n}\n", p->display_record->version);
					versions[pos][x] = p->display_record->version;
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
				int p = get_row_index(tables[pos], row_id);
				if (p == -1)
					printf("Error: Row_ID not found!\n");
				else
				{
					row *dummy = tables[pos]->rows;
					row *link;
					int q = 0;
					while (q < p)
					{
						link = dummy;
						dummy = dummy->next;
						q++;
					}
					if (dummy->next == NULL)
						link->next = NULL;
					else
						link->next = dummy->next;
					free(dummy);
					for (i = p; i < rows[pos] - 1; i++)
						versions[pos][i] = versions[pos][i + 1];
					rows[pos] -= 1;
					versions[pos] = (int*)realloc(versions[pos], sizeof(int)*rows[pos]);
					printf("Record successfully deleted!\n");
				}
			}
		}
		else if (!_strcmpi("FLUSH", op))
			flush_to_file(tables, table_no);
		else if (!_strcmpi("EXIT", op))
			exit(0);
		else if (!_strcmpi("LOAD", op))
		{
			int ch;
			printf("Warning: Any existing data with a version conflict might be overwritten! Proceed(1/0)?: ");
			scanf("%d", &ch);
			if (ch)
			{
				int pos = get_table(tables, table_name, table_no);
				if (pos == -1)
					printf("Error: Table not found! Create table before you flush!\n\n");
				else
				{
					char column[20];
					x = 0;
					while (query[i] != ' '&&query[i] != '\0')
						column[x++] = query[i++];
					column[x] = '\0';
					if (query[i])
						i++;
					else
					{
						printf("Error: Filename not entered\n\n");
						continue;
					}
					char  filename[40];
					x = 0;
					while (query[i] != '\0')
						filename[x++] = query[i++];
					filename[x] = '\0';
					load_from_files(&tables[pos], column, filename,&rows[pos]);
					printf("Load successful!\n");
				}
			}
			else
				printf("Load cancelled\n\n");
			continue;
		}
		else
			printf("Error: Invalid Command!\n\n");
	}
	return 0;
}
