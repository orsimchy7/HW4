
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash-table.h"

/* Hash-table user-function declarations */
int hash_string_func(ht_key_t key, int size);
void* clone_key(void *elem);
void destroy_key(void *a);
int compare_keys(void *a, void *b);
void* clone_student(void *elem);
void destroy_int(void *elem);

/* courses user-function declarations */
int course_clone(void *course, void **courseOutput);

void destroy_course(void* course);


struct student {
	int ID;
	char* name;
	struct list* courses_list;
	struct iterator* iterator; //check if needed here
}

struct course {
	int grade;
	char *courseName;
}


//2 funcs of courses - USER FUNCTIONS for courses list
int course_clone(void *course, void **courseOutput){
	struct course* course_in = (course*)course;

	(course*)(*courseOutput)->grade = course_in->grade;

	course_out->courseName = (char*)malloc(sizeof(course_out->courseName)+1);
	if(!course_out->courseName){
		return 1; //fail
	}

	return 0;
}

//STOPPED HERE




//function that gets user ID string and returns key [0, size-1]
int hash_string_func(ht_key_t key, int size){
    char *str=(char*)key;
    
    // A fast hash function for strings
    int hash=5381;
    for (int i=0; i<strlen(str); ++i) {
        hash = ((hash << 5) + hash) + str[i];
    }

    /* Modulo over hash-size... */
    return hash % size;

}

void* clone_key(void* key){
	char *str=(char*)(key);
    char *out=(char*)malloc(sizeof(char)*(strlen(str)+1));
    if (!out) {
        return NULL;
    }
    strcpy(out, str);
    return out;
}


//void destroy_key(void* key){
//	free(key);
//}

int compare_keys(void *key_1, void *key_2){
	char *str_a=(char*)key_1;
    char *str_b=(char*)key_2;
    return strcmp(str_a,str_b)==0;
}

void* clone_student(void* elem){

	struct student* student_in = (student*)elem;
	struct student* student_out = (student*)malloc(sizeof(student)); //check sizeof(student)

	if(!student_out){
		return NULL;
	}

	student_out->ID = student_in->ID //(char*)malloc(sizeof(student_in->ID)+1);

	student_out->name = (char*)malloc(sizeof(student_in->name)+1);
	if(!student_out->name){
		//free(student_out->ID);
		free(student_out);
		return NULL;
	}

	student_out->courses_list = list_init();



}

