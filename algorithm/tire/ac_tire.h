#ifndef study_ac_tire_h_
#define study_ac_tire_h_

#define MAX_CHILDREN 26

struct ACTireNode;
typedef struct ACTireNode *ACTireTree;

ACTireTree InsertTire(ACTireTree root, const char *str);
ACTireTree DestoryTire(ACTireTree root);
ACTireTree BuildAcAutomation(ACTireTree root);


#endif
