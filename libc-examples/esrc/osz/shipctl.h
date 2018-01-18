int shipctl(char *payload);

struct _menu_table {
    int (*cmdptr)(char *payload);
    char * const command;
    bool cpm_required;
    bool cpm_refuse;
		bool must_select;
};

typedef struct _menu_table menuTable;

