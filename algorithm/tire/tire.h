#ifndef study_tire_h_
#define study_tire_h_

#define MAX_CHILDREN 26

struct TireNode;
typedef struct TireNode *TireTree;

TireTree InsertTire(TireTree root, const char *str);
TireTree DestoryTire(TireTree root);


#endif
