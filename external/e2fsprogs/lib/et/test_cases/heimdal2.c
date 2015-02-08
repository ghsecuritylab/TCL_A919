
#include <stdlib.h>

static const char * const text[] = {
	"$Id: kadm_err.et,v 1.5 1998/01/16 23:11:27 joda Exp $",
	"Cannot fetch local realm",
	"Unable to fetch credentials",
	"Bad key supplied",
	"Can't encrypt data",
	"Cannot encode/decode authentication info",
	"Principal attemping change is in wrong realm",
	"Packet is too large",
	"Version number is incorrect",
	"Checksum does not match",
	"Unsealing private data failed",
	"Unsupported operation",
	"Could not find administrating host",
	"Administrating host name is unknown",
	"Could not find service name in services database",
	"Could not create socket",
	"Could not connect to server",
	"Could not fetch local socket address",
	"Could not fetch master key",
	"Could not verify master key",
	"Entry already exists in database",
	"Database store error",
	"Database read error",
	"Insufficient access to perform requested operation",
	"Data is available for return to client",
	"No such entry in the database",
	"Memory exhausted",
	"Could not fetch system hostname",
	"Could not bind port",
	"Length mismatch problem",
	"Illegal use of wildcard",
	"Database is locked or in use--try again later",
	"Insecure password rejected",
	"Cleartext password and DES key did not match",
	"Invalid principal for change srvtab request",
	"Attempt do delete immutable principal",
	"Reserved kadm error (36)",
	"Reserved kadm error (37)",
	"Reserved kadm error (38)",
	"Reserved kadm error (39)",
	"Reserved kadm error (40)",
	"Reserved kadm error (41)",
	"Reserved kadm error (42)",
	"Reserved kadm error (43)",
	"Reserved kadm error (44)",
	"Reserved kadm error (45)",
	"Reserved kadm error (46)",
	"Reserved kadm error (47)",
	"Reserved kadm error (48)",
	"Reserved kadm error (49)",
	"Reserved kadm error (50)",
	"Reserved kadm error (51)",
	"Reserved kadm error (52)",
	"Reserved kadm error (53)",
	"Reserved kadm error (54)",
	"Reserved kadm error (55)",
	"Reserved kadm error (56)",
	"Reserved kadm error (57)",
	"Reserved kadm error (58)",
	"Reserved kadm error (59)",
	"Reserved kadm error (60)",
	"Reserved kadm error (61)",
	"Reserved kadm error (62)",
	"Reserved kadm error (63)",
	"Null passwords are not allowed",
	"Password is too short",
	"Too few character classes in password",
	"Password is in the password dictionary",
    0
};

struct error_table {
    char const * const * msgs;
    long base;
    int n_msgs;
};
struct et_list {
    struct et_list *next;
    const struct error_table * table;
};
extern struct et_list *_et_list;

const struct error_table et_kadm_error_table = { text, -1783126272L, 68 };

static struct et_list link = { 0, 0 };

void initialize_kadm_error_table_r(struct et_list **list);
void initialize_kadm_error_table(void);

void initialize_kadm_error_table(void) {
    initialize_kadm_error_table_r(&_et_list);
}

/* For Heimdal compatibility */
void initialize_kadm_error_table_r(struct et_list **list)
{
    struct et_list *et, **end;

    for (end = list, et = *list; et; end = &et->next, et = et->next)
        if (et->table->msgs == text)
            return;
    et = malloc(sizeof(struct et_list));
    if (et == 0) {
        if (!link.table)
            et = &link;
        else
            return;
    }
    et->table = &et_kadm_error_table;
    et->next = 0;
    *end = et;
}
