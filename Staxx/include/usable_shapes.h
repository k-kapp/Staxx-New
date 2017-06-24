#pragma once


#include <vector>
#include <memory>

class shape;

void init_imported_shapes();

extern std::vector<std::shared_ptr<shape> > selected_shapes;
extern std::vector<std::shared_ptr<shape> > all_shapes;