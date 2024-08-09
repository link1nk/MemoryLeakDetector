#include "MLD.hpp"
#include <iostream>

#include <memory.h>
#include <stdlib.h>

/*Application Structures*/

typedef struct emp_ {

    char emp_name[30];
    unsigned int emp_id;
    unsigned int age;
    struct emp_* mgr;
    float salary;
} emp_t;

typedef struct student_ {

    char stud_name[32];
    unsigned int rollno;
    unsigned int age;
    float aggregate;
    struct student_* best_colleage;
} student_t;

int main(void)
{
	MLD* MemoryLeakDetector{ new MLD };
	
	static MLD::FieldInfo emp_fields[]
	{
        FIELD_INFO(emp_t, emp_name, MLD::DataType::CHAR,    0),
        FIELD_INFO(emp_t, emp_id,   MLD::DataType::UINT32,  0),
        FIELD_INFO(emp_t, age,      MLD::DataType::UINT32,  0),
        FIELD_INFO(emp_t, mgr,      MLD::DataType::OBJ_PTR, emp_t),
        FIELD_INFO(emp_t, salary,   MLD::DataType::FLOAT,   0)
	};
    REG_STRUCT(MemoryLeakDetector, emp_t, emp_fields);

    
    static MLD::FieldInfo student_fields[]
    {
        FIELD_INFO(student_t, stud_name,     MLD::DataType::CHAR,    0),
        FIELD_INFO(student_t, rollno,        MLD::DataType::UINT32,  0),
        FIELD_INFO(student_t, age,           MLD::DataType::UINT32,  0),
        FIELD_INFO(student_t, aggregate,     MLD::DataType::FLOAT,   0),
        FIELD_INFO(student_t, best_colleage, MLD::DataType::OBJ_PTR, student_t)
    };
    REG_STRUCT(MemoryLeakDetector, student_t, student_fields);

    /*Step 2 : Create some sample objects, equivalent to standard
     * calloc(1, sizeof(student_t))*/
    student_t* Lincoln = (student_t*) MemoryLeakDetector->XCalloc("student_t", 1);
    student_t* LincolnDenovo = (student_t*) MemoryLeakDetector->XCalloc("student_t", 1);
    emp_t* LincolnDenovoDenovo = (emp_t*)MemoryLeakDetector->XCalloc("emp_t", 2);

    std::cout << MemoryLeakDetector;
    return 0;
}