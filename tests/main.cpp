#include "kvdb_lib.hpp"

const char *keys[] = {
    "user.UserName",
    "user.FirstName",
    "user.LastName",
    "user.NSN",
    "user.DateOfBirth",
    "user.Nationality",
    "user.CountryOfOrigin",
    "user.Race",
    "user.CountryLivingIn",
    "user.BloodType",
    "user.School",
    "user.City",
    "user.Suburb",
    "user.Gender",
    "user.HasDriversLicence",
    "user.Hobby"
};
#define KEY_CNT sizeof(keys) / sizeof(char*)

const char *vals[] = {
    "dfgbdfsvfd",
    "fgdfv",
    "fdgfdv",
    "16904120",
    "44545",
    "gfdg",
    "dfgfdsg",
    "fdsgfdsgdfgtrhtgh",
    "rgrttigd",
    "Unknown",
    "66",
    "Athththd",
    "ghgrh",
    "mnnn",
    "ghgfh",
    "C htgfh"
};
#define VAL_CNT sizeof(vals) / sizeof(char*)

#define ENTRY_COUNT KEY_CNT

#define MAX_ENTRY_COUNT (ENTRY_COUNT + 31) & ~31

int main() {
    if (KEY_CNT != VAL_CNT) return EXIT_FAILURE;

    kvdb::KVDBObj db;
    db.new_db("database/kvdat0002.db", MAX_ENTRY_COUNT);
    for (size_t i = 0; i < ENTRY_COUNT; i++) {
        Key *key = kvdb::set_key_str(keys[i]);
        Val *val = kvdb::set_val_str(vals[i]);
        db.insert(*key, *val);
        kvdb::destroy_key(key);
        kvdb::destroy_val(val);
    }
    db.close_db();

    putchar('\n');

    db.open_db("database/kvdat0002.db");
    for (size_t i = 0; i < ENTRY_COUNT; i++) {
        Key *key = kvdb::set_key_str(keys[i]);
        db.read_by_key(*key);
        kvdb::destroy_key(key);
    }
    db.close_db();

    db.open_db("database/kvdat0002.db");
    db.delete_by_id(1);
    db.delete_by_id(21);
    db.delete_by_id(7);
    db.delete_by_id(4);
    db.delete_by_id(13);

    for (size_t i = 0; i < ENTRY_COUNT; i++) {
        Key *key = kvdb::set_key_str(keys[i]);
        db.read_by_key(*key);
        kvdb::destroy_key(key);
    }
    print_dbg_msg("Here\n");

    db.close_db();

    return 0;
}