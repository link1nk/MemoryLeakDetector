#ifndef __MLD_HPP_
#define __MLD_HPP_

#include <string>
#include <ostream>
#include <cstdio>
#include <cstddef>
#include <cassert>
#include "ANSIColors.hpp"

class MLD
{
public:
    class DBrec;
    class FieldInfo;
    class ObjDB;
    class ObjDBrec;

    enum DataType
    {
        UINT8,
        UINT32,
        INT32,
        CHAR,
        OBJ_PTR,
        VOID_PTR,
        FLOAT,
        DOUBLE,
        OBJ_STRUCT
    };

    MLD();
    ~MLD();
	
    int AddStructure(DBrec*);
    void* XCalloc(const char*, uint32_t);

    friend std::ostream& operator<<(std::ostream&, const MLD*);
    friend std::ostream& operator<<(std::ostream&, const MLD::DBrec*);

private:
    ObjDB* ObjectDatabase;
    DBrec* Head;
    size_t Count;

    DBrec*    struct_db_look_up(const char*);
    ObjDBrec* object_db_look_up(void*);
    void      add_object_to_object_db(void* ptr, uint32_t units, DBrec* struct_rec);
}; 

class MLD::FieldInfo
{
public:
    std::string   FName;                     /*Name of the field*/
    MLD::DataType DType;                     /*Data type of the field*/
    uint32_t      Size;                      /*Size of the field*/
    uint32_t      Offset;                    /*Offset of the field*/
    std::string   NestedStrName;             /*Only if dtype = OBJ_PTR, Or OBJ_STRUCT*/

    FieldInfo(std::string FName, MLD::DataType DType, uint32_t Size, uint32_t Offset, std::string NestedStrName);
};

class MLD::DBrec
{
public:
    MLD::DBrec*     Next;                    /* Pointer to the next structure in the linked list */
    std::string     StructName;              /* Key */
    uint32_t        DsSize;                  /* Size of the structure */
    uint32_t        NFields;                 /* No of fields in the structure */
    MLD::FieldInfo* Fields;                  /* Pointer to the array of fields */

    DBrec();
    DBrec(std::string StructName, uint32_t DsSize, uint32_t NFields, MLD::FieldInfo* Fields);
};

#define OFFSETOF(struct_name, fld_name) offsetof(struct_name, fld_name)

#define FIELD_SIZE(struct_name, fld_name)   \
    sizeof(((struct_name *)nullptr)->fld_name)

#define FIELD_INFO(struct_name, fld_name, dtype, nested_struct_name)    \
   {#fld_name, dtype, FIELD_SIZE(struct_name, fld_name),                \
        OFFSETOF(struct_name, fld_name), #nested_struct_name}

#define REG_STRUCT(struct_db, st_name, fields_arr)                                     \
        {                                                                              \
        MLD::DBrec* rec = new MLD::DBrec{ #st_name,                                    \
                                          sizeof(st_name),                             \
                                          sizeof(fields_arr) / sizeof(MLD::FieldInfo), \
                                          fields_arr };                                \
        rec->Next = nullptr;                                                           \
        struct_db->AddStructure(rec);                                                  \
        }

class MLD::ObjDB
{
public:
    MLD*      MemoryLeakDetector;
    ObjDBrec* Head;
    uint32_t  Count;

    friend std::ostream& operator<<(std::ostream&, ObjDB*);

    ObjDB(MLD* MemoryLeakDetector);
};


class MLD::ObjDBrec
{
public:
    ObjDBrec* Next;
    void*     Ptr;
    uint32_t  Units;
    DBrec*    DatabaseRecords;

    ObjDBrec(ObjDBrec*, void*, uint32_t, DBrec*);

    friend std::ostream& operator<<(std::ostream&, ObjDBrec*);
};

#endif // __MLD_HPP_