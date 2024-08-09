#include "MLD.hpp"


MLD::MLD() :
    Head(nullptr), Count(0), ObjectDatabase(new ObjDB{ this })
{}

MLD::~MLD()
{
	// TO-DO
}

int MLD::AddStructure(MLD::DBrec* struct_rec)
{
    MLD::DBrec* head = this->Head;

    if (head == nullptr)
    {
        this->Head = struct_rec;
        struct_rec->Next = nullptr;
        this->Count++;
        return 0;
    }

    struct_rec->Next = head;
    this->Head = struct_rec;
    this->Count++;

    return 0;
}

void* MLD::XCalloc(const char* struct_name, uint32_t units)
{
    MLD::DBrec* struct_rec = struct_db_look_up(struct_name);

    assert(struct_rec);

    void* ptr = calloc(units, struct_rec->DsSize);

    add_object_to_object_db(ptr, units, struct_rec);

    return ptr;
}

MLD::DBrec* MLD::struct_db_look_up(const char* struct_name)
{
    MLD::DBrec* Head = this->Head;
    
    if (Head == nullptr) return nullptr;

    for (; Head; Head = Head->Next)
    {
        if (Head->StructName == struct_name)
            return Head;
    }
    return nullptr;
}

MLD::ObjDBrec* MLD::object_db_look_up(void* ptr)
{
    MLD::ObjDBrec* Head = ObjectDatabase->Head;

    if (Head == nullptr) return nullptr;

    for (; Head; Head = Head->Next)
    {
        if (Head->Ptr == ptr)
            return Head;
    }

    return nullptr;
}

void MLD::add_object_to_object_db(void* ptr, uint32_t units, DBrec* struct_rec)
{
    MLD::ObjDBrec* obj_rec = object_db_look_up(ptr);
    /*Dont add same object twice*/
    assert(!obj_rec);

    obj_rec = new MLD::ObjDBrec{nullptr, ptr, units, struct_rec};

    MLD::ObjDBrec* Head = this->ObjectDatabase->Head;

    if (Head == nullptr) {
        ObjectDatabase->Head = obj_rec;
        obj_rec->Next = nullptr;
        ObjectDatabase->Count++;
        return;
    }

    obj_rec->Next = Head;
    ObjectDatabase->Head = obj_rec;
    ObjectDatabase->Count++;
}

std::ostream& operator<<(std::ostream& os, const MLD* mld)
{
    if (mld == nullptr) return os;

    os << "printing structure db\n";
    os << "No of Structures Registered = " << mld->Count << '\n';

    MLD::DBrec* struct_rec = mld->Head;

    int i = 0;
    while (struct_rec != nullptr)
    {
        os << "Structure No : " << i++ << " (0x" << (void*)struct_rec << ")\n";
        os << struct_rec;
        struct_rec = struct_rec->Next;
    }

    os << mld->ObjectDatabase;

    return os;
}

std::ostream& operator<<(std::ostream& os, const MLD::DBrec* db_rec)
{
    std::string DATA_TYPE[] = { "UINT8", "UINT32", "INT32",
                                "CHAR", "OBJ_PTR", "VOID_PTR", "FLOAT",
                                "DOUBLE", "OBJ_STRUCT" };

    if (!db_rec) return os;

    MLD::FieldInfo* field{ nullptr };

    printf(ANSI_COLOR_CYAN "|------------------------------------------------------|\n" ANSI_COLOR_RESET);

    printf(ANSI_COLOR_YELLOW "| %-20s | size = %-8d | #flds = %-3d |\n" ANSI_COLOR_RESET,
        db_rec->StructName.c_str(), db_rec->DsSize, db_rec->NFields);

    printf(ANSI_COLOR_CYAN "|------------------------------------------------------|"
        "------------------------------------------------------------------------------------------|\n"
        ANSI_COLOR_RESET);

    for (uint32_t j = 0; j < db_rec->NFields; j++)
    {
        field = &db_rec->Fields[j];
        printf("  %-20s |", "");
        printf("%-3d %-20s | dtype = %-15s | size = %-5d | offset = %-6d|  nstructname = %-20s  |\n",
            j, field->FName.c_str(), DATA_TYPE[field->DType].c_str(),
            field->Size, field->Offset, field->NestedStrName.c_str());
        printf("  %-20s |", "");
        printf(ANSI_COLOR_CYAN "------------------------------------------------------------------"
            "--------------------------------------------------------|\n" ANSI_COLOR_RESET);
    }

    return os;
}

MLD::DBrec::DBrec(std::string StructName, uint32_t DsSize, uint32_t NFields, MLD::FieldInfo* Fields) :
	Next(nullptr), StructName(StructName), DsSize(DsSize), NFields(NFields), Fields(Fields)
{}

MLD::DBrec::DBrec() :
	Next(nullptr), StructName(""), DsSize(0), NFields(0), Fields(nullptr)
{}

MLD::FieldInfo::FieldInfo(std::string FName, MLD::DataType DType, uint32_t Size, uint32_t Offset, std::string NestedStrName) :
	FName(FName), DType(DType), Size(Size), Offset(Offset), NestedStrName(NestedStrName)
{}

MLD::ObjDB::ObjDB(MLD* MemoryLeakDetector) :
    Head(nullptr), Count(0), MemoryLeakDetector(MemoryLeakDetector)
{}

MLD::ObjDBrec::ObjDBrec(ObjDBrec* Next, void* Ptr, uint32_t Units, DBrec* DatabaseRecords) :
    Next(Next), Ptr(Ptr), Units(Units), DatabaseRecords(DatabaseRecords)
{}

std::ostream& operator<<(std::ostream& os, MLD::ObjDB* object_db)
{
    MLD::ObjDBrec* Head = object_db->Head;

    os << ANSI_COLOR_CYAN << "Printing OBJECT DATABASE\n";

    for (uint32_t i = 0; Head; Head = Head->Next, i++)
    {
        os << ANSI_COLOR_MAGENTA << "-----------------------------------------------------------------------------------------------|\n";
        printf(ANSI_COLOR_YELLOW "%-3d ptr = ", i);
        os << Head;
        os << ANSI_COLOR_MAGENTA << "-----------------------------------------------------------------------------------------------|\n";
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, MLD::ObjDBrec* obj_rec)
{
    printf("%-10p | next = %-10p | units = %-4d | struct_name = %-10s |\n",
        obj_rec->Ptr, obj_rec->Next, obj_rec->Units, obj_rec->DatabaseRecords->StructName.c_str());

    return os;
}