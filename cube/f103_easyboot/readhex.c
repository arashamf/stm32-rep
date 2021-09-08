/*
 * readhex.c
 * 
 * Copyright 2021 Пользователь <Пользователь@DESKTOP-49E48BL>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef unsigned char uint8_t;

int main()
{
	FILE *fp1; char nameoffile1 [] = "f103_flash.hex";//указатель на файл
	char buffer1 [48]; char buffer2 [48]; char buffer3 [48];//char * ptr = buffer;
	uint8_t id = 0, length = 0; unsigned int ext_adress = 0;
	
	if ((fp1 = fopen (nameoffile1, "r")) == NULL) //функция fopen возвращает указатель на файл или NULL, если не удалось открыть файл. 
	{					//1й аргумент адрес строки, содержащий имя файла, 2й - аргумент, определяющий режим открытия файла
	printf ("Не удаётся открыть %s", nameoffile1); //"r" - открыть тектовый файл только для чтения. 
	exit (1);  //функция закрывает программу, аргумент передаётся ОС, 0 при корректном завершение, 1 - при аварийном.
	}
	fgets (buffer1, 48, fp1);
	printf ("%s\n", buffer1);
	length = 2 * ((buffer1 [1] - 48)*10 + (buffer1 [2] - 48));
	id = buffer1 [8] - 48;
	
	if (id == 4)
		{
		for (uint8_t count = 0; count < length; count++)	
			{
				unsigned int mnogitel = 1;
				for (uint8_t i = 1; i < (length-count); i++)
					mnogitel *= 10;
				ext_adress += (buffer1 [count+9] - 48) * mnogitel;
			//ext_adress += ((buffer1 [count+8] - 48) * (pow (10, (length-count))));
			printf ("%d buffer1 = %d\n",ext_adress, (buffer1 [count+9] - 48));
			}
		}
	printf ("id = %d,  length = %d, ext_adress = %d\n", id, length, ext_adress);
	fgets (buffer2, 48, fp1);
	printf ("%s\n", buffer2);
	fgets (buffer3, 48, fp1);
	printf ("%s\n", buffer3);
	
	fclose (fp1); //ф-я fclose закрывает файл, очищая при этом буферы. Ф-я возвращает 0 в случае успешного закрытия или EOF в противном случае
	
	return 0;
}

