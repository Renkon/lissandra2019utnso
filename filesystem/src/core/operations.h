#ifndef CORE_OPERATIONS2_H_
#define CORE_OPERATIONS2_H_

#include <stdio.h>
#include <string.h>
#include <commons/bitarray.h>
#include "../table_utils.h"
#include "utils/operation_types.h"
#include "generic_logger.h"
#include "../filesystem/filesystem.h"
#include "../directorys.h"
#include "utils/dates.h"
#include "utils/operation_types.h"
#include "utils/response.h"

void process_select(select_input_t* input, response_t* response);
void process_insert(insert_input_t* input, response_t* response);
void process_create(create_input_t* input, response_t* response);
void process_describe(describe_input_t* input, response_t* response);
void process_drop(drop_input_t* input, response_t* response);
void process_value(void* unused, response_t* response);

#endif /* CORE_OPERATIONS2_H_ */
