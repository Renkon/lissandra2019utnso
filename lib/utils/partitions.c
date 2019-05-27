#include "partitions.h"

int calculate_partition_number (int key, int partitions_amount){
	//Calculo en que particion va a estar la key porque esta se guarda en la particion:(key MOD numeroParticiones)+1
	return (key%partitions_amount)+1;

}
